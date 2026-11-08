#include <algds/vector.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct algds_vec {
    void *data;
    size_t size;
    size_t capacity;
    size_t elem_size;
};

algds_status algds_vec_create(algds_vec **out, size_t elem_size) {
    if (out == NULL) {
        return ALGDS_ERR_INVALID_ARG;
    }
    if (elem_size == 0) {
        *out = NULL;
        return ALGDS_ERR_INVALID_ARG;
    }
    *out = (algds_vec *)malloc(sizeof(algds_vec));
    if (!*out)
        return ALGDS_ERR_NOMEM;
    (*out)->data = NULL;
    (*out)->size = 0;
    (*out)->capacity = 0;
    (*out)->elem_size = elem_size;
    return ALGDS_OK;
}

void algds_vec_destroy(algds_vec **v) {
    if (!v || !*v)
        return;
    free((*v)->data);
    free(*v);
    *v = NULL;
}

algds_status algds_vec_push(algds_vec *v, const void *elem) {
    if (!v || !elem)
        return ALGDS_ERR_INVALID_ARG;
    if (v->size >= v->capacity) {
        size_t max_cap = SIZE_MAX / v->elem_size;
        size_t new_capacity = v->capacity;
        if (new_capacity >= max_cap)
            return ALGDS_ERR_NOMEM;
        if (new_capacity == 0)
            new_capacity = 4;
        else if (new_capacity > max_cap / 2)
            new_capacity = max_cap;
        else
            new_capacity *= 2;
        new_capacity = new_capacity > max_cap ? max_cap : new_capacity;
        void *tmp_data = (void *)realloc(v->data, v->elem_size * new_capacity);
        if (!tmp_data)
            return ALGDS_ERR_NOMEM;
        v->data = tmp_data;
        v->capacity = new_capacity;
    }
    unsigned char *dst = (unsigned char *)v->data + v->size * v->elem_size;
    memcpy(dst, elem, v->elem_size);
    v->size++;
    return ALGDS_OK;
}

algds_status algds_vec_get(const algds_vec *v, size_t i, void *out) {
    if (!v || !out)
        return ALGDS_ERR_INVALID_ARG;
    if (i >= v->size)
        return ALGDS_ERR_RANGE;
    if (!v->data)
        return ALGDS_ERR_INVALID_ARG;
    unsigned char *src = (unsigned char *)v->data + i * v->elem_size;
    memcpy(out, src, v->elem_size);
    return ALGDS_OK;
}

size_t algds_vec_size(const algds_vec *v) {
    if (!v)
        return 0;
    return v->size;
}
