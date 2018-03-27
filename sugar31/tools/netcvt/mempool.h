#ifndef __MEMPOOL_H
#define __MEMPOOL_H

/*
 * Simple pool-based memory allocator
 *
 * Compile with MEMPOOL_DEBUG_CLEAR to clear memory immediately before
 * freeing it in order to test for accessing destroyed objects.
 *
 * mempool_create  -- create a new pool with "spans" of a given size.
 *                    A span is the default size of chunks allocated by
 *                    mallocing.
 * mempool_destroy -- free up a pool
 * mempool_get     -- allocate a block of memory
 * mempool_cget    -- allocate and clear a block of memory
 *
 * mempool_geth    -- allocate a block of memory and simultaneously
 *                    save a "handle" containing the previous state
 *                    of the pool.
 * mempool_cgeth   -- allocate and clear a block of memory and record 
 *                    pool state in a handle
 * mempool_freeh   -- free up everything allocated from a geth or cgeth
 *                    onward, and restore the pool state to that stored
 *                    in the appropriate handle.
 *
 * mempool_memdup  -- allocate and copy into a new buffer
 * mempool_strdup  -- allocate and copy a string into a new buffer
 *
 * mempool_start_log -- Start logging memory requests
 * mempool_end_log   -- Stop logging memory requests
 */

typedef struct mempool_struct* mempool_t;

mempool_t mempool_create(int span_size);
void      mempool_destroy(mempool_t pool);
void*     mempool_get(mempool_t pool, int size);
void*     mempool_cget(mempool_t pool, int size);

void*     mempool_geth(mempool_t pool, int size);
void*     mempool_cgeth(mempool_t pool, int size);
void      mempool_freeh(void* mem);

void*     mempool_memdup(mempool_t pool, void* src, int size);
char*     mempool_strdup(mempool_t pool, char* src);

void      mempool_start_log(char* filename);
void      mempool_end_log();


typedef union {
    int i;
    long l;
    long* lp;
    void* p;
    void (*fp)();
    float f;
    double d;
    long double ld;
} align_t;

#define align_size (sizeof(align_t))
#define MEMPOOL_DEFAULT_SPAN (4096)

#endif /* __MEMPOOL_H */

