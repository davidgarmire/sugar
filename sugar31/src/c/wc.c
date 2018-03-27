#include <sugar.h>

#include <stdio.h>
#include <string.h>

#include <hash.h>

typedef struct wc_entry_t {
    char* string;
    int count;
} wc_entry_t;

int wc_entry_compare(const void* x, const void* y)
{
    return strcmp( ((wc_entry_t*) x)->string, 
                   ((wc_entry_t*) y)->string );
}

unsigned wc_entry_hash(const void* key)
{
    return hash_strhash(((wc_entry_t*) key)->string);
}

void* wc_entry_copy(mempool_t pool, const void* data)
{
    wc_entry_t* entry;

    entry = (wc_entry_t*) mempool_memdup(pool, (void*) data, sizeof(wc_entry_t));
    entry->string = mempool_strdup(pool, entry->string);
    return entry;
}

hash_t wc_hash_create(int size_hint)
{
    return hash_create(size_hint, 
                       wc_entry_compare, 
                       wc_entry_hash,
                       wc_entry_copy);
}

void count_file(FILE* fp, hash_t hash)
{
    char buf[128];

    wc_entry_t key;
    key.string = buf;

    while (fscanf(fp, "%s", buf) != EOF) {
        wc_entry_t* entry = (wc_entry_t*) hash_retrieve(hash, &key);
        if (entry == NULL) {
            wc_entry_t new_entry;
            new_entry.string = buf;
            new_entry.count  = 1;
            hash_insert(hash, &new_entry);
        } else {
            entry->count++;
        }
    }   
}

void print_entry(void* data)
{
    wc_entry_t* entry = (wc_entry_t*) data;
    printf("%03d: %s\n", entry->count, entry->string);
}

int main(int argc, char** argv)
{
    hash_t hash = wc_hash_create(1000);
    --argc, ++argv;
    while (argc != 0) {
        FILE* fp = fopen(*argv, "r");
        count_file(fp, hash);
        fclose(fp);
        --argc, ++argv;
    }
    hash_doall(hash, print_entry);
    hash_destroy(hash);
    return 0;
}

