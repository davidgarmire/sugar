#include <sugar.h>

#include <assert.h>
#include <string.h>

#include <mempool.h>
#include <hash.h>

int stdcall_strcmp(const char* a, const char* b)
{
    return strcmp(a, b);
}
typedef struct hash_entry_t hash_entry_t;

struct hash_t {
    int             size;
    hash_entry_t**  table;
    mempool_t       pool;
    int             own_pool_flag;

    hash_compare_fun_t compare;
    hash_hash_fun_t    hash;
    hash_copy_fun_t    copy;
};

struct hash_entry_t {
    struct hash_entry_t* next;
    void* data;
};

hash_t hash_create(int size_hint,
                   hash_compare_fun_t compare,
                   hash_hash_fun_t    hash,
                   hash_copy_fun_t    copy)
{
    mempool_t pool = mempool_create(MEMPOOL_DEFAULT_SPAN);
    hash_t    h    = hash_pcreate(pool, size_hint, compare, hash, copy);
    h->own_pool_flag = 1;
    return h;
}

hash_t hash_pcreate(mempool_t pool,
                    int size_hint,
                    hash_compare_fun_t compare,
                    hash_hash_fun_t    hash,
                    hash_copy_fun_t    copy)
{
    static int primes[] = {119,  253,  509,   1021,  2053, 
                           4093, 8191, 16381, 32771, 65521};
    static int num_primes = 10;

    hash_t h = (hash_t) mempool_get(pool, sizeof(struct hash_t));
    int i;

    for (i = 0; i < num_primes && size_hint < primes[i]; ++i);
    h->size          = primes[i];
    h->pool          = pool;
    h->own_pool_flag = 1;
    h->compare  = compare;
    h->hash     = hash;
    h->copy     = copy;
    h->table    = (hash_entry_t**) 
                  mempool_cget(pool, h->size * sizeof(hash_entry_t*));
    return h;
}

void hash_destroy(hash_t hash)
{
    if (hash->own_pool_flag)
        mempool_destroy(hash->pool);
}

hash_t hstrings_create(int size_hint)
{
    return hash_create(size_hint,
                       (hash_compare_fun_t) stdcall_strcmp,
                       (hash_hash_fun_t)    hash_strhash,
                       (hash_copy_fun_t)    mempool_strdup);
}

hash_t hstrings_pcreate(mempool_t pool, int size_hint)
{
    return hash_pcreate(pool,
                        size_hint,
                        (hash_compare_fun_t) stdcall_strcmp,
                        (hash_hash_fun_t)    hash_strhash,
                        (hash_copy_fun_t)    mempool_strdup);
}

unsigned hash_strhash(const char* key)
{
    unsigned g, h = 0;
    for (; *key != '\0'; ++key) {
        h = (h << 4) + *key;
        g = h & 0xF0000000;
        if (g) {
            h = h ^ (g >> 24);
            h = h ^ g;
        }
    }
    return h;   
}

unsigned hash_bytehash(const void* key, int n)
{
    const unsigned char* s = (const unsigned char*) key;
    unsigned h = 0;
    while (n-- > 0) {
        h *= 5;
        h += *s++;
    }
    return h;
}

static hash_entry_t* hash_search_bucket(hash_t hash, int bucket,
                                                const void* key)
{
    hash_entry_t* entry = hash->table[bucket];

    while (entry != NULL && (hash->compare)(entry->data, key) != 0)
        entry = entry->next;

    return entry;
}

void* hash_insert(hash_t hash, void* data)
{
    if (data == NULL) {
        return NULL;
    } else {
        int index = (hash->hash)(data) % (hash->size);
        hash_entry_t* entry = hash_search_bucket(hash, index, data);

        if (hash->copy)
            data = (hash->copy)(hash->pool, data);

        if (entry != NULL) {
            void* old_data = entry->data;
            entry->data = data;
            return old_data;
        } else {
            entry = (hash_entry_t*) mempool_get(hash->pool, sizeof(hash_entry_t));
            entry->data = data;
            entry->next = hash->table[index];
            hash->table[index] = entry;
            return NULL;
        }
    }
}

void* hash_remove(hash_t hash, const void* key)
{
    if (key == NULL) {
        return NULL;
    } else {
        int index = (hash->hash)(key) % (hash->size);

        hash_entry_t* prev  = NULL;
        hash_entry_t* entry = hash->table[index];

        while (entry != NULL && (hash->compare)(entry->data, key) != 0) {
            prev  = entry;
            entry = entry->next;
        }
        
        if (entry == NULL)
            return NULL;
        
        if (prev == NULL)
            hash->table[index] = entry->next;
        else
            prev->next = entry->next;
        
        return entry->data;
    }
}

void* hash_retrieve(hash_t hash, const void* key)
{
    if (key == NULL) {
        return NULL;
    } else {
        int index = (hash->hash)(key) % (hash->size);
        hash_entry_t* entry = hash_search_bucket(hash, index, key);
        if (entry == NULL)
            return NULL;
        else
            return entry->data;
    }
}

void* hash_add(hash_t hash, void* data)
{
    if (data == NULL) {
        return NULL;
    } else {
        int index = (hash->hash)(data) % (hash->size);
        hash_entry_t* entry = hash_search_bucket(hash, index, data);
        if (entry == NULL) {
            if (hash->copy)
                data = (hash->copy)(hash->pool, data);
            entry = (hash_entry_t*) mempool_get(hash->pool, sizeof(hash_entry_t));
            entry->data = data;
            entry->next = hash->table[index];
            hash->table[index] = entry;
        } 
        return entry->data;
    }
}
void hash_doall(hash_t hash, void (*func)(void* data))
{
    int bucket;
    hash_entry_t* entry;

    for (bucket = 0; bucket < hash->size; ++bucket)
        for (entry = hash->table[bucket]; entry != NULL; entry = entry->next)
            (func)(entry->data);
}

void hash_doall_arg(hash_t hash, 
                            void (*func)(void* data, void* arg),
                            void* arg)
{
    int bucket;
    hash_entry_t* entry;

    for (bucket = 0; bucket < hash->size; ++bucket)
        for (entry = hash->table[bucket]; entry != NULL; entry = entry->next)
            (func)(entry->data, arg);
}

