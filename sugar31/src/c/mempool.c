#include <sugar.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <mempool.h>

#define MEMPOOL_MAGIC_NUMBER 0x1234
#define ASSERT_VALID_POOL \
    assert(pool != NULL && pool->magic_number == MEMPOOL_MAGIC_NUMBER)

struct mempool_struct {
    int magic_number;
    int blocks_per_span;
    int blocks_used;
    align_t* first_span;
    align_t* current_span;
};

#define MEMPOOL_HANDLE_MAGIC_NUMBER 0xA911

#define MEMPOOL_HANDLE_SIZE \
   (((sizeof(mempool_handle_t) + align_size-1) / align_size) * align_size)

typedef struct mempool_handle_t {
    int magic_number;
    int blocks_used;
    align_t* current_span;
    mempool_t pool;
} mempool_handle_t;

static FILE* mempool_log_fp = NULL;

static align_t* allocate_span(mempool_t pool, int blocks_in_span)
{
    align_t* span;

    if (blocks_in_span < pool->blocks_per_span)
        blocks_in_span = pool->blocks_per_span;
    span = (align_t*) malloc((1+blocks_in_span) * align_size);
    #ifdef MEMPOOL_CLEAR
    memset(span, 0, (1+blocks_in_span) * align_size);
    #endif
    assert(span != NULL);
    span[0].p = NULL;

    if (mempool_log_fp)
        fprintf(mempool_log_fp, "%p: Created new span at %p\n",
                (void*) pool, (void*) span);

    return span;
}

mempool_t mempool_create(int span_size)
{
    mempool_t pool;

    assert( span_size > 0 );
    assert( pool = (mempool_t) malloc(sizeof(struct mempool_struct)) );

    pool->magic_number = MEMPOOL_MAGIC_NUMBER;
    pool->blocks_per_span = (span_size + align_size-1)/align_size;
    pool->first_span = allocate_span(pool, 0);
    pool->current_span = pool->first_span;
    pool->blocks_used = 0;
    if (mempool_log_fp)
        fprintf(mempool_log_fp, "Allocated new mempool (%p), span size %d\n",
                (void*) pool, span_size);

    return pool;
}

void mempool_destroy(mempool_t pool)
{
    align_t* next_span;

    ASSERT_VALID_POOL;
    next_span = pool->first_span;
    while (next_span != NULL) {
        align_t* dead_span = next_span;
        next_span = (align_t*) next_span[0].p;
        #ifdef MEMPOOL_CLEAR_DEBUG
        memset(dead_span, 0, (1 + pool->blocks_per_span)*align_size);
        #endif
        free(dead_span);
    }
    pool->magic_number = 0;
    free(pool);

    if (mempool_log_fp)
        fprintf(mempool_log_fp, "Destroyed mempool (%p)\n", (void*) pool);
}

void* mempool_get(mempool_t pool, int size)
{
    int blocks_requested = (size + align_size-1) / align_size;
    void* mem;

    ASSERT_VALID_POOL;
    assert(blocks_requested >= 0);

    if (pool->blocks_used + blocks_requested > pool->blocks_per_span) {
        align_t* new_span = allocate_span(pool, blocks_requested);
        pool->current_span[0].p = new_span;
        pool->current_span = new_span;
        pool->blocks_used = 0;
    }
    mem = &(pool->current_span[1 + pool->blocks_used]);
    pool->blocks_used += blocks_requested;

    if (mempool_log_fp)
        fprintf(mempool_log_fp,
                "%p: Allocated %d bytes / %d blocks at %p\n",
                (void*) pool, size, blocks_requested * align_size,
                mem);

    return mem;
}

void* mempool_cget(mempool_t pool, int size)
{
    void* mem = mempool_get(pool, size);
    memset(mem, 0, size);
    return mem;
}

void* mempool_geth(mempool_t pool, int size)
{
    mempool_handle_t new_handle;
    char* mem;

    ASSERT_VALID_POOL;

    new_handle.magic_number = MEMPOOL_HANDLE_MAGIC_NUMBER;
    new_handle.blocks_used  = pool->blocks_used;
    new_handle.current_span = pool->current_span;
    new_handle.pool         = pool;
    mem = (char*) mempool_get(pool, size + MEMPOOL_HANDLE_SIZE);
    *((mempool_handle_t*) mem) = new_handle;
    mem += MEMPOOL_HANDLE_SIZE;

    if (mempool_log_fp)
        fprintf(mempool_log_fp,
                "%p: Handle at %p, memory at %p\n",
                (void*) pool, (void*) (mem - MEMPOOL_HANDLE_SIZE), (void*) mem);

    return mem;
}

void* mempool_cgeth(mempool_t pool, int size)
{
    void* mem = mempool_geth(pool, size);
    memset(mem, 0, size);
    return mem;
}

void mempool_freeh(void* mem)
{
    mempool_handle_t handle;
    mempool_t pool;
    align_t* next_span;

    assert(mem != NULL);
    handle = *(mempool_handle_t*) ((char*) mem - MEMPOOL_HANDLE_SIZE);
    pool = handle.pool;
    assert(handle.magic_number == MEMPOOL_HANDLE_MAGIC_NUMBER);
    ASSERT_VALID_POOL;
    next_span = handle.current_span[0].p;
    while (next_span != NULL) {
        align_t* dead_span = next_span;
        next_span = next_span[0].p;
        #ifdef MEMPOOL_CLEAR_DEBUG
        memset(dead_span, 0, (1 + pool->blocks_per_span)*align_size);
        #endif
        free(dead_span);
    }
    pool->blocks_used  = handle.blocks_used;
    pool->current_span = handle.current_span;
    pool->current_span[0].p = NULL;

    #ifdef MEMPOOL_CLEAR_DEBUG
    memset(&(pool->current_span[1 + pool->blocks_used]), 0,
           (pool->blocks_per_span - pool->blocks_used) * align_size);
    #endif

    if (mempool_log_fp)
        fprintf(mempool_log_fp,
                "%p: Rollback to handle at %p\n", (void*) pool,
                (void*) ((char*) mem - MEMPOOL_HANDLE_SIZE));
}

void* mempool_memdup(mempool_t pool, const void* src, int size)
{
    if (src == NULL)
        return NULL;
    else {
        void* dest = mempool_get(pool, size);
        memcpy(dest, src, size);
        return dest;
    }
}

void* mempool_strdup(mempool_t pool, const char* src)
{
    if (src == NULL)
        return NULL;
    return mempool_memdup(pool, src, strlen(src) + 1);
}

void mempool_start_log(char* filename)
{
    assert(mempool_log_fp == NULL);
    mempool_log_fp = fopen(filename, "a");
    assert(mempool_log_fp != NULL);
    fprintf(mempool_log_fp, "Memory log started (%s)\n", filename);
}

void mempool_end_log()
{
    assert(mempool_log_fp != NULL);
    fprintf(mempool_log_fp, "Closing memory log\n");
    fclose(mempool_log_fp);
    mempool_log_fp = NULL;
}

void mempool_flush_log()
{
    assert(mempool_log_fp != NULL);
    fflush(mempool_log_fp);
}

