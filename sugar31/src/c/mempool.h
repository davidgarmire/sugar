#ifndef MEMPOOL_H
#define MEMPOOL_H

typedef struct mempool_struct* mempool_t;

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

mempool_t mempool_create (int span_size);
void      mempool_destroy(mempool_t pool);

void* mempool_get  (mempool_t pool, int size);
void* mempool_cget (mempool_t pool, int size);
void* mempool_geth (mempool_t pool, int size);
void* mempool_cgeth(mempool_t pool, int size);
void  mempool_freeh(void* mem);

void* mempool_memdup(mempool_t pool, const void* src, int size);
void* mempool_strdup(mempool_t pool, const char* src);

void mempool_start_log(char* filename);
void mempool_end_log  ();
void mempool_flush_log();

void mempool_persist(mempool_t pool);


#endif /* MEMPOOL_H */
