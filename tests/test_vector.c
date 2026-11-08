#include "algds/error.h"
#include "test_harness.h"
#include <algds/vector.h>

typedef struct {
    char c;
    double d;
    int i;
} toy_t;

int main(void) {
    int failed = 0;
    algds_vec *v = NULL;

    /* 1. Create and destroy basics */
    CHECK(algds_vec_create(&v, sizeof(int)) == ALGDS_OK);
    REQUIRE(v != NULL);
    CHECK(algds_vec_size(v) == 0);
    algds_vec_destroy(&v);
    CHECK(v == NULL);

    /* 2. Bad elem_size */
    v = (algds_vec *)1;
    CHECK(algds_vec_create(&v, 0) == ALGDS_ERR_INVALID_ARG);
    CHECK(v == NULL);

    /* 3. Size after each push */
    CHECK(algds_vec_create(&v, sizeof(int)) == ALGDS_OK);
    REQUIRE(v != NULL);
    for (int i = 0; i < 5; i++) {
        int val = i;
        CHECK(algds_vec_push(v, &val) == ALGDS_OK);
        CHECK(algds_vec_size(v) == (size_t)(i + 1));
    }
    algds_vec_destroy(&v);

    /* 4. Many ints across several growths */
    CHECK(algds_vec_create(&v, sizeof(int)) == ALGDS_OK);
    REQUIRE(v != NULL);
    for (int i = 0; i < 1000; i++) {
        int val = i * 3;
        CHECK(algds_vec_push(v, &val) == ALGDS_OK);
        CHECK(algds_vec_size(v) == (size_t)(i + 1));
    }
    for (int i = 0; i < 1000; i++) {
        int out = 0;
        CHECK(algds_vec_get(v, (size_t)i, &out) == ALGDS_OK);
        CHECK(out == i * 3);
    }
    algds_vec_destroy(&v);

    /* 5. Struct element */
    CHECK(algds_vec_create(&v, sizeof(toy_t)) == ALGDS_OK);
    REQUIRE(v != NULL);
    toy_t items[3] = {{'a', 1.1, 10}, {'b', 2.2, 20}, {'c', 3.3, 30}};
    for (int i = 0; i < 3; i++) {
        CHECK(algds_vec_push(v, &items[i]) == ALGDS_OK);
    }
    for (int i = 0; i < 3; i++) {
        toy_t out;
        CHECK(algds_vec_get(v, (size_t)i, &out) == ALGDS_OK);
        CHECK(out.c == items[i].c);
        CHECK(out.d == items[i].d);
        CHECK(out.i == items[i].i);
    }
    algds_vec_destroy(&v);

    /* 6. Out of range */
    CHECK(algds_vec_create(&v, sizeof(int)) == ALGDS_OK);
    REQUIRE(v != NULL);
    {
        int out = 0;
        // get on an empty vector returns ALGDS_ERR_RANGE
        CHECK(algds_vec_get(v, 0, &out) == ALGDS_ERR_RANGE);

        int val = 42;
        CHECK(algds_vec_push(v, &val) == ALGDS_OK);

        // i == size returns RANGE (off-by-one boundary)
        CHECK(algds_vec_get(v, 1, &out) == ALGDS_ERR_RANGE);

        // i == size - 1 returns OK
        CHECK(algds_vec_get(v, 0, &out) == ALGDS_OK);
        CHECK(out == 42);
    }
    algds_vec_destroy(&v);

    /* 7. destroy(NULL) */
    algds_vec_destroy(NULL);
    algds_vec *none = NULL;
    algds_vec_destroy(&none);
    CHECK(none == NULL);

    /* 8. size(NULL) == 0 */
    CHECK(algds_vec_size(NULL) == 0);

    /* OPTIONAL: NULL arguments contract checks */
    {
        int x = 10;
        CHECK(algds_vec_push(NULL, &x) == ALGDS_ERR_INVALID_ARG);

        CHECK(algds_vec_create(&v, sizeof(int)) == ALGDS_OK);
        REQUIRE(v != NULL);
        CHECK(algds_vec_push(v, NULL) == ALGDS_ERR_INVALID_ARG);
        CHECK(algds_vec_get(NULL, 0, &x) == ALGDS_ERR_INVALID_ARG);
        CHECK(algds_vec_get(v, 0, NULL) == ALGDS_ERR_INVALID_ARG);
        algds_vec_destroy(&v);
    }

    return TEST_STATUS;
}
