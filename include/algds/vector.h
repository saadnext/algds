#ifndef ALGDS_VECTOR_H
#define ALGDS_VECTOR_H

#include <algds/error.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file vector.h
 * @brief Dynamic array of fixed-size elements, stored contiguously.
 *
 * Elements are stored by value: the vector copies `elem_size` bytes in and
 * out, and never looks inside them. The element size is fixed at creation.
 *
 * Thread-safety: a vector must not be accessed from several threads if any
 * of them modifies it. Concurrent calls to read-only functions (get, size)
 * on a vector that nobody is modifying are safe.
 */

/** Opaque handle. Its fields are private to the library. */
typedef struct algds_vec algds_vec;

/**
 * @brief Creates an empty vector.
 *
 * @param[out] out        Receives the new vector. Must not be NULL.
 * @param[in]  elem_size  Size in bytes of one element. Must be > 0.
 *
 * @return ALGDS_OK             Success. `*out` is a new vector with size 0.
 * @return ALGDS_ERR_INVALID_ARG `out` is NULL (nothing is written), or
 *                              `elem_size` is 0 (`*out` is set to NULL).
 * @return ALGDS_ERR_NOMEM      Allocation failed. `*out` is set to NULL.
 *
 * @note Ownership: the caller owns the vector and must release it with
 *       algds_vec_destroy().
 * @note Whenever `out` is non-NULL, `*out` is either a valid vector or NULL
 *       afterwards, never garbage.
 * @note Complexity: O(1).
 */
algds_status algds_vec_create(algds_vec **out, size_t elem_size);

/**
 * @brief Destroys a vector and sets the caller's pointer to NULL.
 *
 * @param[in,out] v  Address of the caller's vector pointer.
 *
 * @note If `v` is NULL, or `*v` is NULL, this does nothing.
 * @note Otherwise releases the vector and its element storage, then sets
 *       `*v = NULL`. Other copies of the pointer are NOT updated and
 *       dangle after this call.
 * @note Elements are plain bytes. If they contain pointers to memory the
 *       caller allocated, the caller must free that memory first.
 * @note Complexity: O(1).
 */
void algds_vec_destroy(algds_vec **v);

/**
 * @brief Appends a copy of an element to the end of the vector.
 *
 * @param[in,out] v     The vector. Must not be NULL.
 * @param[in]     elem  Points to `elem_size` readable bytes. Must not be
 *                      NULL. The vector copies the bytes and keeps no
 *                      reference to `elem`.
 *
 * @return ALGDS_OK             Success. Size increased by 1.
 * @return ALGDS_ERR_INVALID_ARG `v` or `elem` is NULL. Vector unchanged.
 * @return ALGDS_ERR_NOMEM      Allocation failed or the required storage size
 *                              would overflow. Vector unchanged and still
 *                              fully usable.
 *
 * @note Complexity: amortized O(1). A single call that has to grow the
 *       storage is O(n) in the current size.
 */
algds_status algds_vec_push(algds_vec *v, const void *elem);

/**
 * @brief Copies the element at index `i` into a caller-provided buffer.
 *
 * @param[in]  v    The vector. Must not be NULL.
 * @param[in]  i    Zero-based index. Must be < algds_vec_size(v).
 * @param[out] out  Points to at least `elem_size` writable bytes. Must not
 *                  be NULL.
 *
 * @return ALGDS_OK             Success. `elem_size` bytes were written to `out`.
 * @return ALGDS_ERR_INVALID_ARG `v` or `out` is NULL. `out` untouched.
 * @return ALGDS_ERR_RANGE      `i >= size`. `out` untouched.
 *
 * @note The element is copied, so the caller never holds a pointer into the
 *       vector's storage, and later pushes cannot invalidate anything the
 *       caller received.
 * @note Complexity: O(1) (one copy of `elem_size` bytes).
 */
algds_status algds_vec_get(const algds_vec *v, size_t i, void *out);

/**
 * @brief Returns the number of elements currently stored.
 *
 * @param[in] v  The vector. NULL is allowed.
 *
 * @return The element count, or 0 if `v` is NULL.
 *
 * @note Complexity: O(1).
 */
size_t algds_vec_size(const algds_vec *v);

#ifdef __cplusplus
}
#endif

#endif /* ALGDS_VECTOR_H */
