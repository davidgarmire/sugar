#ifndef HASH_H
#define HASH_H

#include "mempool.h"

typedef struct hash_t* hash_t;

typedef int      (*hash_compare_fun_t)(const void* x, const void* y);
typedef unsigned (*hash_hash_fun_t)   (const void* key);
typedef void*    (*hash_copy_fun_t)   (mempool_t pool, const void* data);

hash_t hash_create (int size_hint,
                    hash_compare_fun_t compare,
                    hash_hash_fun_t    hash,
                    hash_copy_fun_t    copy);
hash_t hash_pcreate(mempool_t pool,
                    int size_hint,
                    hash_compare_fun_t compare,
                    hash_hash_fun_t    hash,
                    hash_copy_fun_t    copy);
void   hash_destroy(hash_t hash);

unsigned hash_strhash (const char* key);
unsigned hash_bytehash(const void* key, int n);

void* hash_insert  (hash_t hash, void* data);
void* hash_remove  (hash_t hash, const void* key);
void* hash_retrieve(hash_t hash, const void* key);
void* hash_add     (hash_t hash, void* data);

void hash_doall    (hash_t hash, void (*func)(void* data));
void hash_doall_arg(hash_t hash, void (*func)(void* data, void* arg),
                    void* arg);

hash_t  hstrings_create (int size_hint);
hash_t  hstrings_pcreate(mempool_t pool, int size_hint);
#define hstrings_add(h, s)      ((const char*) (hash_add(h, (void*) s)))
#define hstrings_retrieve(h, s) ((const char*) hash_retrieve(h, s))

#endif /* HASH_H */
