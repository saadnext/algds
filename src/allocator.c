#include <algds/allocator.h>
#include <stddef.h>
#include <stdlib.h>

static void *alloc_fn(void *ctx, size_t size) {
    (void)ctx;
    return malloc(size);
}

static void *realloc_fn(void *ctx, void *ptr, size_t size) {
    (void)ctx;
    return realloc(ptr, size);
}

static void free_fn(void *ctx, void *ptr) {
    (void)ctx;
    free(ptr);
}

algds_allocator algds_default_allocator(void) {
    algds_allocator allocator;
    allocator.alloc_fn = alloc_fn;
    allocator.realloc_fn = realloc_fn;
    allocator.free_fn = free_fn;
    allocator.ctx = NULL;
    return allocator;
}
