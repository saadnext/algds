#ifndef ALGDS_TEST_HARNESS_H
#define ALGDS_TEST_HARNESS_H

#include <stdio.h>  /* IWYU pragma: keep */
#include <string.h> /* IWYU pragma: keep */

/**
 * @brief Evaluates the `failed` counter and returns the appropriate process exit code.
 * @note Requires a valid `int failed` variable in scope.
 *       Returns 0 (Success) if no failures occurred, or 1 (Failure) if any check failed.
 */
#define TEST_STATUS (failed > 0 ? 1 : 0)

/**
 * @brief Asserts that a condition is true.
 * @note Requires a mutable `int failed` variable to be defined within the calling scope.
 *       This variable will be incremented by 1 if the assertion fails.
 *
 * @param cond The boolean expression to evaluate.
 */
#define CHECK(cond)                                                                                \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            fprintf(stderr, "CHECK failed at %s:%d\n", __FILE__, __LINE__);                        \
            fprintf(stderr, "  Expression: %s\n", #cond);                                          \
            failed++;                                                                              \
        }                                                                                          \
    } while (0)

/**
 * @brief Asserts a critical condition is true. Aborts the current function on failure.
 * @note Requires a mutable `int failed` variable in scope.
 */
#define REQUIRE(cond)                                                                              \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            fprintf(stderr, "REQUIRE failed (FATAL) at %s:%d\n", __FILE__, __LINE__);              \
            fprintf(stderr, "  Expression: %s\n", #cond);                                          \
            failed++;                                                                              \
            return TEST_STATUS; /* Bails out immediately, preserving the failure state */          \
        }                                                                                          \
    } while (0)

/**
 * @brief Asserts that two C-strings are structurally equal.
 * @note Requires a mutable `int failed` variable to be defined within the calling scope.
 *       This variable will be incremented by 1 if the assertion fails.
 * @note Uses ultra-unique suffixes to entirely eliminate the risk of -Wshadow variable collision.
 *
 * @param a The expected string (handles NULL safely).
 * @param b The actual string (handles NULL safely).
 */
#define CHECK_STR_EQ(a, b)                                                                         \
    do {                                                                                           \
        const char *str_a_cseq_7x8 = (a);                                                          \
        const char *str_b_cseq_7x8 = (b);                                                          \
        if (str_a_cseq_7x8 == NULL || str_b_cseq_7x8 == NULL) {                                    \
            if (str_a_cseq_7x8 != str_b_cseq_7x8) {                                                \
                fprintf(stderr, "CHECK_STR_EQ failed at %s:%d\n", __FILE__, __LINE__);             \
                fprintf(stderr, "  Expected: %s\n", str_a_cseq_7x8 ? str_a_cseq_7x8 : "NULL");     \
                fprintf(stderr, "  Actual:   %s\n", str_b_cseq_7x8 ? str_b_cseq_7x8 : "NULL");     \
                failed++;                                                                          \
            }                                                                                      \
        } else if (strcmp(str_a_cseq_7x8, str_b_cseq_7x8) != 0) {                                  \
            fprintf(stderr, "CHECK_STR_EQ failed at %s:%d\n", __FILE__, __LINE__);                 \
            fprintf(stderr, "  Expected: \"%s\"\n", str_a_cseq_7x8);                               \
            fprintf(stderr, "  Actual:   \"%s\"\n", str_b_cseq_7x8);                               \
            failed++;                                                                              \
        }                                                                                          \
                                                                                                   \
    } while (0)

#endif /* ALGDS_TEST_HARNESS_H */
