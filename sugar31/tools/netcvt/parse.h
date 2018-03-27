#ifndef __PARSE_H
#define __PARSE_H

/* Data structures for the high-level tree representation of the netlist,
 * and associated function.  The routines in parse.[ch] do most of the 
 * work for the parse actions in sugar.y.
 *
 *  sugar_parse -- scan, parse, and return the tree representation
 *                 for a netlist
 *  add_error_atcode -- add an error message at the location of a
 *                      code object
 *  get_num_ids -- return the number of definitions in the current scope;
 *                 when called after parsing, returns the number of
 *                 definitions in the global scope
 *
 * The remaining routines are primarily for keeping track of parse state
 * and assembling the tree structure.  They should probably not be used
 * outside parse.[ch] and sugar.y.
 */

#include <stdio.h>
#include "mempool.h"

/* Function call node */
struct call_struct {
    char* name;                 /* Name of function to call */
    struct code_struct* args;   /* List of arguments to the call */
};

/* Variable / parameter definition */
struct def_struct {
    char* name;                 /* Name of variable/param */
    struct code_struct* value;  /* Value (NULL if none bound) */
    int id;                     /* Unique ID number per variable in scope */
    int scope;                  /* 0 for global scope; 1 for subnet */
};

/* Process definition */
struct process_struct {
    char* name;                 /* Name of process structure */
    struct code_struct* defs;   /* List of value bindings */
    struct code_struct* parent; /* Parent process */
};

/* Element/node name node */
struct name_struct {
    char* name;                  /* Unadorned local name of node */
    struct code_struct* indices; /* Any associated indices */
};

/* Element node */
struct element_struct {
    char* model;                    /* Name of the model function */
    struct code_struct* name;       /* Name of the element */
    struct code_struct* process;    /* Process structure to use */
    struct code_struct* nodes;      /* Nodes element affects */
    struct code_struct* params;     /* Model parameters */
    struct code_struct* model_node; /* Subnet pointer (for subnet instances) */
};

/* For node */
struct for_struct {
    struct code_struct* idx;        /* Index variable declaration */
    struct code_struct* lower;      /* Lower bound of index */
    struct code_struct* upper;      /* Upper bound of index */
    struct code_struct* body;       /* Body of for loop */
};

/* If node */
struct if_struct {
    struct code_struct* cond;        /* Condition */
    struct code_struct* then_clause; /* Clause to put in if true */
    struct code_struct* else_clause; /* Clause to put in otherwise */
};

/* Subnet node */
struct subnet_struct {
    char* model;                    /* Subnet model name */
    struct code_struct* nodes;      /* Formal names of nodes */
    struct code_struct* params;     /* Formal parameters and defaults */
    struct code_struct* body;       /* Subnet body */
    int def_count;                  /* Number of local variables */
};

enum NODE_TYPES {
    INT_NODE,
    DOUBLE_NODE,
    STRING_NODE,
    UNDEF_NODE,
    CALL_NODE,
    VAR_NODE,
    DEF_NODE,
    PARAM_NODE,
    PROCESS_NODE,
    NAME_NODE,
    ELEMENT_NODE,
    FOR_NODE,
    IF_NODE,
    SUBNET_NODE,
    BLOCK_NODE
};

/* Code node */
typedef struct code_struct {
    int tag;
    union {
        struct  call_struct         call;
        struct  code_struct*        var;
        struct  def_struct          def;
        struct  process_struct      process;
        struct  name_struct         name;
        struct  element_struct      element;
        struct  for_struct          forloop;
        struct  if_struct           ifstmt;
        struct  subnet_struct       subnet;
        struct  code_struct*        block;
        double                      dval;
        int                         ival;
	char*                       sval;
    } v;
    int lineno;
    int fileno;
    char* token;
    struct code_struct* next;
} code_t;

/* Call the parser */

code_t* sugar_parse(mempool_t pool);

/* Manage symbols */

extern code_t* null_process;   /* Process object for "null" (*) process */
extern code_t* parent_process; /* Process object for subnet "parent" process */

/* Add an error message at the location of a code object */
int add_error_atcode(code_t* code, char* errmsg);
int add_warning_atcode(code_t* code, char* errmsg);

void parse_init();
code_t* get_precode();
void push_level();
void pop_level();
void start_subnet();
void end_subnet();
void add_var(char* name, code_t* code);
void add_process(char* name, code_t* code);
void add_model(char* name, code_t* code);
code_t* lookup_var(char* name);
code_t* lookup_process(char* name);
code_t* lookup_model(char* name);
int get_num_ids();

/* Create and concatenate nodes */

code_t* new_node(int tag);
code_t* cat_nodes(code_t* nodes1, code_t* nodes2);

code_t* float_node(double dval, char* s);
code_t* string_node(char* sval);
code_t* undef_node();
code_t* var_node(char* name);
code_t* call_node(char* name, code_t* args);
code_t* def_node(char* name, code_t* value);
code_t* param_node(char* name, code_t* value);
code_t* process_node(char* name, code_t* defs, char* parent_name);
code_t* name_node(char* name, code_t* indices);
code_t* block_node(code_t* block);
code_t* elt_node_head(char* basename, code_t* indices, char* model, 
                      code_t* process);
code_t* elt_node_finish(code_t* elt, code_t* nodes, code_t* params);
code_t* subnet_node(char* model, code_t* nodes, code_t* params);
code_t* subnet_body(code_t* subnet, code_t* body);
code_t* for_node(char* var, code_t* lower, code_t* upper);
code_t* for_node_body(code_t* node, code_t* body);
code_t* if_node(code_t* cond, code_t* then_clause, code_t* else_clause);

#endif /* __PARSE_H */
