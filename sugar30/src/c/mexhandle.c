#include <stdio.h>

#include "mex-handle.h"
#include "dynarray.h"

typedef struct mex_handle_t {
    void* data;
    int   tag;
    int   id;
} mex_handle_t;

static dynarray_t handles;
static int next_tag;
static int next_id;
static int free_list;

void mex_handle_create()
{
    handles   = dynarray_create(sizeof(mex_handle_t), 16);
    next_tag  = 1;
    next_id   = 1;
    free_list = -1;
}

void mex_handle_destroy()
{
    dynarray_destroy(handles);
}

int mex_handle_new_tag()
{
    return next_tag++;
}

int mex_handle_add(void* data, int tag)
{
    int index;
    mex_handle_t* handle;

    if (free_list >= 0) {
        index      = free_list;
        handle     = dynarray_get(handles, index);
        free_list  = handle->id;
    } else {
        index      = dynarray_count(handles);
        handle     = dynarray_append(handles, NULL);
    }

    handle->data = data;
    handle->tag  = tag;
    handle->id   = (next_tag++ << 8) + (index & 255);

    return handle->id;
}

void* mex_handle_remove(int handle_id)
{
    int index;
    mex_handle_t* handle;
    void*         data;

    for (index = (handle_id & 255);
         (handle = dynarray_get(handles, index)) != NULL &&
             handle->id != handle_id;
         index += 256);

    if (handle == NULL)
        return NULL;
    else {
        data = handle->data;
        handle->tag  = 0;
        handle->data = NULL;
        handle->id   = free_list;
        free_list    = index;

        return data;
    }
}

int mex_handle_find(void* data, int tag)
{
    int i, n;
    mex_handle_t* handle_data = dynarray_data(handles);

    n = dynarray_count(handles);
    for (i = 0; i < n; ++i) {
        if (handle_data[i].tag == tag && handle_data[i].data == data)
            return handle_data[i].id;
    }
    return 0;
}

int mex_handle_tag(int handle_id)
{
    int index;
    mex_handle_t* handle;

    for (index = (handle_id & 255);
         (handle = dynarray_get(handles, index)) != NULL &&
             handle->id != handle_id;
         index += 256);

    return (handle != NULL) ? handle->tag : 0;
}

void* mex_handle_data(int handle_id)
{
    int index;
    mex_handle_t* handle;

    for (index = (handle_id & 255);
         (handle = dynarray_get(handles, index)) != NULL &&
             handle->id != handle_id;
         index += 256);

    return (handle != NULL) ? handle->data : NULL;
}

