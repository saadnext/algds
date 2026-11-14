#ifndef ALGDS_ALLOCATOR_H
#define ALGDS_ALLOCATOR_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Allocates a block of at least `size` bytes. Behaves like malloc().
 *
 * @param ctx   The `ctx` stored in the algds_allocator, passed unchanged.
 * @param size  Number of bytes requested.
 * @return A pointer to the block, or NULL on failure.
 */
typedef void *(*algds_alloc_fn)(void *ctx, size_t size);

/**
 * @brief Resizes a block to `size` bytes. Behaves like realloc().
 *
 * @param ctx   The `ctx` stored in the algds_allocator, passed unchanged.
 * @param ptr   A block from this allocator, or NULL.
 * @param size  New size in bytes.
 * @return The (possibly moved) block, or NULL on failure.
 *
 * @note If `ptr` is NULL this must act like algds_alloc_fn. The library
 *       relies on it for the first growth of an empty vector.
 * @note On failure the original block must stay valid and unchanged. The
 *       library relies on it to leave a vector intact when growth fails.
 */
typedef void *(*algds_realloc_fn)(void *ctx, void *ptr, size_t size);

/**
 * @brief Releases a block. Behaves like free().
 *
 * @param ctx  The `ctx` stored in the algds_allocator, passed unchanged.
 * @param ptr  A block from this allocator, or NULL.
 *
 * @note `ptr == NULL` must be a no-op. The library calls this with NULL
 *       when it destroys a vector that never allocated element storage.
 */
typedef void (*algds_free_fn)(void *ctx, void *ptr);

/**
 * @brief A set of memory functions plus a context pointer.
 *
 * @note All three function pointers must be non-NULL. A block must be
 *       released with the same allocator that produced it.
 * @note `ctx` may be NULL. The library never reads or frees it. It only
 *       passes it as the first argument of every call.
 * @note A vector stores a copy of this struct, so the caller's struct may
 *       go away. Whatever `ctx` points at must outlive the vector.
 * @note The library adds no locking. If the functions touch shared state,
 *       making them thread-safe is the allocator's job.
 */
typedef struct {
    algds_alloc_fn alloc_fn;
    algds_realloc_fn realloc_fn;
    algds_free_fn free_fn;
    void *ctx;
} algds_allocator;

/**
 * @brief Returns the default allocator, built on malloc, realloc and free.
 *
 * @return An algds_allocator whose `ctx` is NULL.
 *
 * @note Ownership: the result is a plain value. There is nothing to free.
 * @note Complexity: O(1).
 */
algds_allocator algds_default_allocator(void);

#ifdef __cplusplus
}
#endif

#endif /* ALGDS_ALLOCATOR_H */
