#include "algds/error.h"
#include "test_harness.h"
#include <algds/allocator.h>
#include <algds/vector.h>
#include <stdlib.h>

typedef struct {
    char c;
    double d;
    int i;
} toy_t;

static void algds_vec_create_test(void) {
    algds_vec *v = NULL;
    CHECK(algds_vec_create(&v, sizeof(int)) == ALGDS_OK);
    REQUIRE(v != NULL);
    CHECK(algds_vec_size(v) == 0);
    algds_vec_destroy(&v);
    CHECK(v == NULL);
    /* 2. Bad elem_size */
    v = (algds_vec *)1;
    CHECK(algds_vec_create(&v, 0) == ALGDS_ERR_INVALID_ARG);
    CHECK(v == NULL);
}

static void algds_vec_push_test(void) {
    algds_vec *v = NULL;
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
}

static void algds_vec_struct_test(void) {
    algds_vec *v = NULL;
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
}

static void algds_vec_out_of_range_test(void) {
    algds_vec *v = NULL;
    CHECK(algds_vec_create(&v, sizeof(int)) == ALGDS_OK);
    REQUIRE(v != NULL);
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

    algds_vec_destroy(&v);
}

static void algds_vec_null_arguments_test(void) {
    algds_vec_destroy(NULL);
    algds_vec *none = NULL;
    algds_vec_destroy(&none);
    CHECK(none == NULL);
    CHECK(algds_vec_size(NULL) == 0);
    int x = 10;
    algds_vec *v = NULL;
    CHECK(algds_vec_push(NULL, &x) == ALGDS_ERR_INVALID_ARG);
    CHECK(algds_vec_create(&v, sizeof(int)) == ALGDS_OK);
    REQUIRE(v != NULL);
    CHECK(algds_vec_push(v, NULL) == ALGDS_ERR_INVALID_ARG);
    CHECK(algds_vec_get(NULL, 0, &x) == ALGDS_ERR_INVALID_ARG);
    CHECK(algds_vec_get(v, 0, NULL) == ALGDS_ERR_INVALID_ARG);
    algds_vec_destroy(&v);
}

/* ---- fake allocator: counts calls, fails exactly one chosen call ---- */
typedef struct {
    int calls;   /* alloc + realloc calls so far */
    int fail_at; /* the call number that returns NULL; 0 = never fail */
    int live;    /* blocks currently allocated and not yet freed */
} alloc_state;

static void *fake_alloc(void *ctx, size_t size) {
    alloc_state *st = ctx;
    st->calls++;
    if (st->calls == st->fail_at)
        return NULL;
    void *p = malloc(size);
    if (p)
        st->live++;
    return p;
}

static void *fake_realloc(void *ctx, void *ptr, size_t size) {
    alloc_state *st = ctx;
    st->calls++;
    if (st->calls == st->fail_at)
        return NULL;
    void *p = realloc(ptr, size);
    if (p && ptr == NULL) /* realloc(NULL, n) creates a new block */
        st->live++;
    return p;
}

static void fake_free(void *ctx, void *ptr) {
    alloc_state *st = ctx;
    if (ptr != NULL) {
        st->live--;
        free(ptr);
    }
}

static algds_allocator make_fake(alloc_state *st) {
    algds_allocator a;
    a.alloc_fn = fake_alloc;
    a.realloc_fn = fake_realloc;
    a.free_fn = fake_free;
    a.ctx = st;
    return a;
}

/* create_ex(NULL allocator) behaves like create */
static void algds_vec_null_allocator_test(void) {
    algds_vec *v = NULL;
    CHECK(algds_vec_create_ex(&v, sizeof(int), NULL) == ALGDS_OK);
    REQUIRE(v != NULL);
    int x = 7, out = 0;
    CHECK(algds_vec_push(v, &x) == ALGDS_OK);
    CHECK(algds_vec_get(v, 0, &out) == ALGDS_OK);
    CHECK(out == 7);
    algds_vec_destroy(&v);
}

/* the handle allocation (call 1) fails */
static void algds_vec_create_fails_test(void) {
    alloc_state st = {0, 1, 0};
    algds_allocator a = make_fake(&st);
    algds_vec *v = (algds_vec *)1; /* garbage: create must overwrite it */
    CHECK(algds_vec_create_ex(&v, sizeof(int), &a) == ALGDS_ERR_NOMEM);
    CHECK(v == NULL);
    CHECK(st.live == 0);
}

/* call 1 = handle, call 2 = growth 0->4, call 3 = growth 4->8 (fails) */
static void algds_vec_push_fails_and_recovers_test(void) {
    alloc_state st = {0, 3, 0};
    algds_allocator a = make_fake(&st);
    algds_vec *v = NULL;
    CHECK(algds_vec_create_ex(&v, sizeof(int), &a) == ALGDS_OK);
    REQUIRE(v != NULL);
    for (int i = 0; i < 4; i++) {
        int val = i * 10;
        CHECK(algds_vec_push(v, &val) == ALGDS_OK);
    }
    int fifth = 40;
    CHECK(algds_vec_push(v, &fifth) == ALGDS_ERR_NOMEM);
    CHECK(algds_vec_size(v) == 4); /* unchanged */
    for (int i = 0; i < 4; i++) {
        int out = -1;
        CHECK(algds_vec_get(v, (size_t)i, &out) == ALGDS_OK);
        CHECK(out == i * 10); /* old elements intact */
    }
    /* the fake fails only call 3, so a retry succeeds */
    CHECK(algds_vec_push(v, &fifth) == ALGDS_OK);
    CHECK(algds_vec_size(v) == 5);
    int out = -1;
    CHECK(algds_vec_get(v, 4, &out) == ALGDS_OK);
    CHECK(out == 40);
    algds_vec_destroy(&v);
    CHECK(st.live == 0);
}

/* the very first growth (data == NULL) fails, then works */
static void algds_vec_first_growth_fails_test(void) {
    alloc_state st = {0, 2, 0};
    algds_allocator a = make_fake(&st);
    algds_vec *v = NULL;
    CHECK(algds_vec_create_ex(&v, sizeof(int), &a) == ALGDS_OK);
    REQUIRE(v != NULL);
    int val = 5, out = 0;
    CHECK(algds_vec_push(v, &val) == ALGDS_ERR_NOMEM);
    CHECK(algds_vec_size(v) == 0);
    CHECK(algds_vec_get(v, 0, &out) == ALGDS_ERR_RANGE);
    CHECK(algds_vec_push(v, &val) == ALGDS_OK);
    algds_vec_destroy(&v); /* data was NULL for a while: free(NULL) path */
    CHECK(st.live == 0);
}

/* success path: every block is released through the fake */
static void algds_vec_no_leaks_test(void) {
    alloc_state st = {0, 0, 0};
    algds_allocator a = make_fake(&st);
    algds_vec *v = NULL;
    CHECK(algds_vec_create_ex(&v, sizeof(int), &a) == ALGDS_OK);
    REQUIRE(v != NULL);
    for (int i = 0; i < 1000; i++) {
        CHECK(algds_vec_push(v, &i) == ALGDS_OK);
    }
    CHECK(st.calls > 0); /* proves the fake was really used */
    CHECK(st.live == 2); /* handle + data block */
    algds_vec_destroy(&v);
    CHECK(st.live == 0);
}

/* the vector keeps its own copy of the allocator struct */
static void algds_vec_allocator_copy_test(void) {
    alloc_state st = {0, 0, 0};
    algds_allocator a = make_fake(&st);
    algds_vec *v = NULL;
    CHECK(algds_vec_create_ex(&v, sizeof(int), &a) == ALGDS_OK);
    REQUIRE(v != NULL);
    a.alloc_fn = NULL; /* wreck the caller's struct */
    a.realloc_fn = NULL;
    a.free_fn = NULL;
    a.ctx = NULL;
    int val = 1;
    CHECK(algds_vec_push(v, &val) == ALGDS_OK);
    algds_vec_destroy(&v);
    CHECK(st.live == 0);
}

/* two vectors, two allocators: one fails, the other doesn't */
static void algds_vec_independent_allocators_test(void) {
    alloc_state sa = {0, 1, 0}; /* A: fails at once */
    alloc_state sb = {0, 0, 0}; /* B: never fails */
    algds_allocator aa = make_fake(&sa);
    algds_allocator ab = make_fake(&sb);
    algds_vec *va = NULL;
    algds_vec *vb = NULL;
    CHECK(algds_vec_create_ex(&va, sizeof(int), &aa) == ALGDS_ERR_NOMEM);
    CHECK(algds_vec_create_ex(&vb, sizeof(int), &ab) == ALGDS_OK);
    REQUIRE(vb != NULL);
    algds_vec_destroy(&vb);
    CHECK(sb.live == 0);
}

int main(void) {
    algds_vec_create_test();
    algds_vec_push_test();
    algds_vec_struct_test();
    algds_vec_out_of_range_test();
    algds_vec_null_arguments_test();
    algds_vec_null_allocator_test();
    algds_vec_create_fails_test();
    algds_vec_push_fails_and_recovers_test();
    algds_vec_first_growth_fails_test();
    algds_vec_no_leaks_test();
    algds_vec_allocator_copy_test();
    algds_vec_independent_allocators_test();
    return TEST_STATUS;
}
