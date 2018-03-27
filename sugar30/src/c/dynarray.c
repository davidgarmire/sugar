#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "dynarray.h"

struct dynarray_t {
    char*  data;
    int    element_size;
    int    count;
    int    capacity;
};

dynarray_t dynarray_create(int element_size, int capacity)
{
    dynarray_t self;

    assert(element_size > 0 && capacity > 0);

    self = (dynarray_t) calloc(1, sizeof(struct dynarray_t));
    assert(self != NULL);

    self->data = (char*) calloc(capacity, element_size);
    assert(self->data != NULL);

    self->element_size = element_size;
    self->capacity = capacity;
    self->count = 0;
    return self;
}

void dynarray_destroy(dynarray_t self)
{
    free(self->data);
    free(self);
}

void* dynarray_data(dynarray_t self)
{
    assert(self != NULL);
    return self->data;
}

int dynarray_count(dynarray_t self)
{
    assert(self != NULL);
    return self->count;
}

void dynarray_set_count(dynarray_t self, int n)
{
    assert(self != NULL && n >= 0);

    if (n > self->capacity) {
        int   new_capacity = (n > self->capacity*2) ? n : (self->capacity*2);
        char* new_data     = (char*) calloc(new_capacity, self->element_size);

        assert(new_data != NULL);
        memcpy(new_data, self->data, self->count * self->element_size);
        free(self->data);

        self->data = new_data;
        self->capacity = new_capacity;
    }

    if (n < self->count) {
        memset(&(self->data[n * self->element_size]), 0, 
                (self->count - n) * self->element_size);
    }

    self->count = n;
}

void* dynarray_get(dynarray_t self, int i)
{
    assert(self != NULL);

    if (i >= self->count || i < 0)
        return NULL;

    return &(self->data[i * self->element_size]);
}

void* dynarray_set(dynarray_t self, int i, const void* data)
{
    assert(self != NULL && i >= 0);

    if (i >= self->count)
        dynarray_set_count(self, i+1);

    if (data != NULL)
        memcpy(&(self->data[i * self->element_size]), data, 
               self->element_size);

    return &(self->data[i * self->element_size]);
}

void* dynarray_append(dynarray_t self, const void* data)
{
    return dynarray_set(self, self->count, data);
}

