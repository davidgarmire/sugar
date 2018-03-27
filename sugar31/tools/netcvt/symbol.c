#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "mempool.h"
#include "symbol.h"

/* Simplified symbol table implementation.  Symbols are stored in
 * a linked-list stack.  I think it's sort of obvious what a symbol
 * is, but the implementation of scoping levels probably bears some
 * explaining.  A level records the state of the symbol table when
 * a new lexical scope is started.  It stores the state, the previous
 * level, and (implicitly) the state of the memory pool at the time
 * it was created.  To pop a level, the explicit state (top symbol in
 * and the structure for the previous level) are restored from the level
 * structure, and then the implicit state is restored (all the memory
 * from the allocations for this level is released).
 */

typedef struct symbol_struct {
    char*  name;                /* Name / key for the symbol */
    void*  data;                /* Pointer to external data */
    struct symbol_struct* next; /* Symbol below this on the stack */
} symbol_t;

typedef struct symtable_level_struct {
    symbol_t* symbols;                   /* Symbols before this level */
    struct symtable_level_struct* next;  /* Previous level */
} symtable_level_t;

struct symtable_struct {
    int       magic_number;
    mempool_t pool;             /* Allocation pool for creating links */
    symbol_t* symbols;          /* Symbol stack */
    symtable_level_t* level;    /* Current scoping level */
};

#define SYMTABLE_MAGIC_NUMBER 0x6382
#define ASSERT_VALID_SYMTABLE \
    assert(symtable != NULL && symtable->magic_number == SYMTABLE_MAGIC_NUMBER)

/* Create a new symbol table
 */
symtable_t symtable_create(void)
{
    mempool_t pool = mempool_create(MEMPOOL_DEFAULT_SPAN);
    symtable_t symtable = 
            (symtable_t) mempool_get(pool, sizeof(struct symtable_struct));

    symtable->magic_number = SYMTABLE_MAGIC_NUMBER;
    symtable->pool = pool;
    symtable->symbols = NULL;
    symtable->level = NULL;

    return symtable;
}

/* Destroy a symbol table
 */
void symtable_destroy(symtable_t symtable)
{
    ASSERT_VALID_SYMTABLE;
    symtable->magic_number = 0x0000;
    mempool_destroy(symtable->pool);
}

/* Start a new scoping level in the symbol table
 */
void symtable_push_level(symtable_t symtable)
{
    symtable_level_t* new_level;
    
    ASSERT_VALID_SYMTABLE;
    new_level = (symtable_level_t*) 
            mempool_geth(symtable->pool, sizeof(symtable_level_t));
    new_level->symbols = symtable->symbols;
    new_level->next = symtable->level;
    symtable->level = new_level;
}

/* Pop a scoping level in the symbol table
 */
void symtable_pop_level(symtable_t symtable)
{
    symtable_level_t* dead_level;

    ASSERT_VALID_SYMTABLE;
    assert(symtable->level != NULL);
    dead_level = symtable->level;
    symtable->symbols = dead_level->symbols;
    symtable->level = dead_level->next;
    mempool_freeh(dead_level);
}

/* Add a new symbol in the current scope.
 */
void symtable_push(symtable_t symtable, char* name, void* data)
{
    symbol_t* s;

    ASSERT_VALID_SYMTABLE;

    s = (symbol_t*) mempool_get(symtable->pool, sizeof(symbol_t));
    s->name = name;
    s->data = data;
    s->next = symtable->symbols;
    symtable->symbols = s;
}

/* Look up symbol.  Return symbol data if the key is found, NULL ow
 */
void* symtable_find(symtable_t symtable, char* name)
{
    symbol_t* s;

    ASSERT_VALID_SYMTABLE;
    
    s = symtable->symbols;
    while (s != NULL && strcmp(s->name, name) != 0)
        s = s->next;
    if (s == NULL)
        return NULL;
    return s->data;
}
