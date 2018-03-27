#ifndef __SYMBOL_H
#define __SYMBOL_H

/* Extremely simple scoped symbol table.  Stores names and pointers to
 * data... does not store copies of symbol data.
 *
 * symtable_create      -- Create a new symbol table
 * symtable_destroy     -- Destroy a symbol table
 * symtable_push_level  -- Push a scoping level onto the table
 * symtable_pop_level   -- Pop off a scope level, and all the symbols in it
 * symtable_push        -- Add a symbol to the table
 * symtable_find        -- Find a symbol
 */

typedef struct symtable_struct* symtable_t;

symtable_t symtable_create    (void);
void       symtable_destroy   (symtable_t symtable);
void       symtable_push_level(symtable_t symtable);
void       symtable_pop_level (symtable_t symtable);
void       symtable_push      (symtable_t symtable, char* name, void* data);
void*      symtable_find      (symtable_t symtable, char* name);

#endif /* __SYMBOL_H */
