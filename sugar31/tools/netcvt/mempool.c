#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "mempool.h"

#define MEMPOOL_MAGIC_NUMBER 0x1234
#define ASSERT_VALID_POOL assert(pool != NULL && \
                                 pool->magic_number == MEMPOOL_MAGIC_NUMBER)

#define MEMPOOL_HANDLE_MAGIC_NUMBER 0xA911
#define MEMPOOL_HANDLE_BLOCKS \
    ((sizeof(mempool_handle_t)+sizeof(align_t)-1) / sizeof(align_t))
#define MEMPOOL_HANDLE_SIZE \
    (MEMPOOL_HANDLE_BLOCKS * sizeof(align_t))

struct mempool_struct {
    int magic_number;
    int blocks_per_span;
    int blocks_used;
    align_t* first_span;
    align_t* current_span;
};

typedef struct mempool_handle_struct {
    int magic_number;
    int blocks_used;
    mempool_t pool;
    align_t* current_span;
} mempool_handle_t;

static FILE* mempool_log_fp = NULL;

/* Allocate and initialize a span of 
 * max(blocks_in_span, pool->blocks_per_span user blocks
 */
static align_t* allocate_span(mempool_t pool, int blocks_in_span)
{
    align_t* span;

    /* Allocate enough space for desired data, plus a bit for
     * the leading pointer */
    if (blocks_in_span < pool->blocks_per_span)
        blocks_in_span = pool->blocks_per_span;

    span = (align_t*) malloc((1 + blocks_in_span) * sizeof(align_t));
#ifdef MEMPOOL_CLEAR_DEBUG
    memset(span, 0, (1 + blocks_in_span) * sizeof(align_t));
#endif
    assert(span != NULL);
    span[0].p = NULL;

    return span;
}

/* Create a new memory pool with the given default span size
 */
mempool_t mempool_create(int span_size)
{
    int blocks_per_span = (span_size + align_size-1)/align_size;
    mempool_t pool = (mempool_t) malloc(sizeof(struct mempool_struct));

    assert(span_size > 0);
    assert(pool != NULL);

    pool->magic_number = MEMPOOL_MAGIC_NUMBER;
    pool->blocks_per_span = blocks_per_span;
    pool->first_span = allocate_span(pool, 0);
    pool->current_span = pool->first_span;
    pool->blocks_used = 0;

    if (mempool_log_fp)
        fprintf(mempool_log_fp, "Allocated new mempool, span size %d\n", 
                span_size);

    return pool;
}

/* Destroy a memory pool
 */
void mempool_destroy(mempool_t pool)
{
    align_t* next_span;

    ASSERT_VALID_POOL;

    next_span = pool->first_span;
    while (next_span != NULL) {
        align_t* dead_span = next_span;
        next_span = (align_t*) dead_span[0].p;
#ifdef MEMPOOL_CLEAR_DEBUG
        memset(dead_span, 0, (1 + pool->blocks_per_span) * sizeof(align_t));
#endif
        free(dead_span);
    }
    free(pool);

    if (mempool_log_fp)
        fprintf(mempool_log_fp, "Destroyed mempool\n");
}

/* Get at least size bytes from the memory pool
 */
void* mempool_get(mempool_t pool, int size)
{
    int blocks_requested = (size + align_size - 1) / align_size;
    void* mem;

    ASSERT_VALID_POOL;

    /* If the request doesn't fit in this block, start a new one. */    
    if (pool->blocks_used + blocks_requested > pool->blocks_per_span) {
        align_t* new_span = allocate_span(pool, blocks_requested);
        pool->current_span[0].p = new_span;
        pool->current_span = new_span;
        pool->blocks_used = 0;

        if (mempool_log_fp)
            fprintf(mempool_log_fp, "Created new block at %p\n", 
	            (void*) new_span);
    }

    /* Allocate a chunk out of the current block */
    mem = &(pool->current_span[1 + pool->blocks_used]);
    pool->blocks_used += blocks_requested;

    if (mempool_log_fp)
        fprintf(mempool_log_fp, "Request %d got %d bytes / %d blocks at %p\n", 
                size, blocks_requested*sizeof(align_t), blocks_requested, mem);

    return mem;
}

/* Get and clear at least size bytes
 */
void* mempool_cget(mempool_t pool, int size)
{
    void* mem = mempool_get(pool, size);
    memset(mem, 0, size);
    return mem;
}

/* Get at least size bytes from the memory pool, and store a handle so the
 * corresponding mempool_freeh request can restore the pool state.
 */
void* mempool_geth(mempool_t pool, int size)
{
    int blocks_used;
    align_t* current_span;
    char* mem;
    mempool_handle_t* handle;

    ASSERT_VALID_POOL;

    blocks_used = pool->blocks_used;
    current_span = pool->current_span;
    mem = mempool_get(pool, size + MEMPOOL_HANDLE_SIZE);
    handle = (mempool_handle_t*) mem;

    handle->magic_number = MEMPOOL_HANDLE_MAGIC_NUMBER;
    handle->pool = pool;
    handle->blocks_used = blocks_used;
    handle->current_span = current_span;
    mem += MEMPOOL_HANDLE_SIZE;

    if (mempool_log_fp)
        fprintf(mempool_log_fp, "Saved handle at %p, returned mem at %p\n", 
                (void*) handle, (void*) mem);

    return mem;
}

/* Get and clear at least size bytes, and store a handle
 */
void* mempool_cgeth(mempool_t pool, int size)
{
    void* mem = mempool_geth(pool, size);
    memset(mem, 0, size);
    return mem;
}

/* Free everything from the handle associated with a geth or cgeth onward
 */
void mempool_freeh(void* mem)
{
    char* handle_mem = ((char*) mem) - MEMPOOL_HANDLE_SIZE;
    mempool_handle_t* handle = (mempool_handle_t*) handle_mem;
    mempool_t pool;
    int blocks_used;
    align_t* current_span;
    align_t* next_span;

    assert(handle != NULL && 
           handle->magic_number == MEMPOOL_HANDLE_MAGIC_NUMBER &&
           handle->pool->magic_number == MEMPOOL_MAGIC_NUMBER);

    pool = handle->pool;
    current_span = handle->current_span;
    blocks_used = handle->blocks_used;

    next_span = handle->current_span[0].p;
    while (next_span != NULL) {
        align_t* dead_span = next_span;
        next_span = (align_t*) dead_span[0].p;
#ifdef MEMPOOL_CLEAR_DEBUG
        memset(dead_span, 0, (1 + pool->blocks_per_span) * sizeof(align_t));
#endif
        free(dead_span);
    }

    pool->blocks_used = blocks_used;
    pool->current_span = current_span;
    pool->current_span[0].p = NULL;

#ifdef MEMPOOL_CLEAR_DEBUG
    {   /* Clear the remainder of the current block */
        align_t* mem = &(pool->current_span[1 + pool->blocks_used]);
        memset(mem, 0, (pool->blocks_per_span-pool->blocks_used)*
                        sizeof(align_t));
    }
#endif
}

void* mempool_memdup(mempool_t pool, void* src, int size)
{
    void* dest = mempool_get(pool, size);
    memcpy(dest, src, size);
    return dest;
}

/* Allocate a string buffer and copy into it
 */
char* mempool_strdup(mempool_t pool, char* src)
{
    int size = strlen(src) + 1;
    char* dest = mempool_get(pool, size);
    memcpy(dest, src, size);
    return dest;
}

/* Turn on logging, directed to file filename.  Logging must not be
 * on already when this function is called.
 */
void mempool_start_log(char* filename)
{
    assert(mempool_log_fp == NULL);
    mempool_log_fp = fopen(filename, "w");
    assert(mempool_log_fp != NULL);
    fprintf(mempool_log_fp, "Memory log started\n");
}

/* Turn off logging of memory pool activity.
 */
void mempool_end_log()
{
    assert(mempool_log_fp != NULL);
    fprintf(mempool_log_fp, "Memory log ended\n");
    fclose(mempool_log_fp);
    mempool_log_fp = NULL;
}
