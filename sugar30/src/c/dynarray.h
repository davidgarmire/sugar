#ifndef DYNARRAY_H
#define DYNARRAY_H

typedef struct dynarray_t* dynarray_t;

dynarray_t dynarray_create (int element_size, int capacity);
void       dynarray_destroy(dynarray_t self);

void*      dynarray_data  (dynarray_t self);
void*      dynarray_get   (dynarray_t self, int i);
void*      dynarray_set   (dynarray_t self, int i, const void* data);
void*      dynarray_append(dynarray_t self, const void* data);

int        dynarray_count    (dynarray_t self);
void       dynarray_set_count(dynarray_t self, int n);

#endif /* DYNARRAY_H */
