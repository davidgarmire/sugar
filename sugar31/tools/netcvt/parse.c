#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "parse.h"
#include "lexer.h"
#include "symbol.h"

/* This is really obnoxious, but MSC apparently thinks it doesn't need to
 * implement the standard C headers.  In particular, it doesn't define M_PI.
 * Meh.
 */
#ifndef M_PI
#define M_PI 3.1415926535897932385
#endif

code_t* null_process;
code_t* parent_process;

static int in_subnet;
static int num_anon_elts;
static int global_num_ids;
static int subnet_num_ids;
static symtable_t var_symbols;
static symtable_t process_symbols;
static symtable_t subnet_symbols;
static symtable_t node_symbols;
static mempool_t parse_pool;

int add_error_atcode(code_t* code, char* errmsg)
{
    return add_error(code->fileno, code->lineno, NULL, errmsg, 0);
}

int add_warning_atcode(code_t* code, char* errmsg)
{
    return add_error(code->fileno, code->lineno, NULL, errmsg, 1);
}

/* --- Manage symbols --- */

void parse_init(mempool_t pool)
{
    in_subnet = 0;
    global_num_ids = 0;
    num_anon_elts = 0;

    parse_pool      = pool;

    var_symbols     = symtable_create();
    process_symbols = symtable_create();
    subnet_symbols  = symtable_create();
    node_symbols    = symtable_create();

    null_process    = process_node("*", NULL, NULL);
    parent_process  = process_node("parent", NULL, NULL);
}

/* Code to prepend into the file */
code_t* get_precode()
{
    code_t* precode = NULL;
    precode = def_node("pi", float_node(M_PI, "3.1415926535897932385")); 
    /* precode = def_node("pi", float_node(180, "180")); */
    return precode;
}

void push_level()
{
    symtable_push_level(var_symbols);
    symtable_push_level(node_symbols);
}

void pop_level()
{
    symtable_pop_level(node_symbols);
    symtable_pop_level(var_symbols);
}

void start_subnet()
{
    push_level();
    in_subnet = 1;
    subnet_num_ids = 0;
}

void end_subnet()
{
    in_subnet = 0;
    pop_level();
}

void add_var(char* name, code_t* code)
{
    symtable_push(var_symbols, name, code);
}

void add_process(char* name, code_t* code)
{
    symtable_push(process_symbols, name, code);
}

void add_model(char* name, code_t* code)
{
    symtable_push(subnet_symbols, name, code);
}

void add_node_names(code_t* nodes)
{
    while (nodes != NULL) {
        symtable_push(node_symbols, nodes->v.name.name, nodes);
        nodes = nodes->next;
    }
}

code_t* lookup_var(char* name)
{
    return (code_t*) symtable_find(var_symbols, name);
}

code_t* lookup_process(char* name)
{
    if (in_subnet && (strcmp(name, "parent") == 0)) {
        return parent_process;
    }
    return (code_t*) symtable_find(process_symbols, name);
}

code_t* lookup_model(char* name)
{
    return (code_t*) symtable_find(subnet_symbols, name);
}

code_t* lookup_node_name(char* name)
{
    return (code_t*) symtable_find(node_symbols, name);
}

int get_num_ids()
{
    return (in_subnet ? subnet_num_ids : global_num_ids);
}

/* --- Create and concatenate generic nodes --- */

code_t* new_node(int tag)
{
    code_t* node = (code_t*) mempool_cget(parse_pool, sizeof(code_t));
    node->tag = tag;
    get_parse_position(&node->lineno, &node->fileno);
    node->token = NULL;
    return node;
}

code_t* cat_nodes(code_t* nodes1, code_t* nodes2)
{
    code_t* tail;
    if (nodes1 == NULL)
        return nodes2;
    for (tail = nodes1; tail->next != NULL; tail = tail->next);
    tail->next = nodes2;
    return nodes1;
}

/* --- Build subtrees for expressions, def lists, etc. --- */

code_t* float_node(double dval, char* s)
{
    code_t* node = new_node(DOUBLE_NODE);
    node->v.dval = dval;
    if (s)
        node->token = strdup(s);
    return node;
}

code_t* string_node(char* sval)
{
    code_t* node = new_node(STRING_NODE);
    node->v.sval = sval;
    return node;
}

code_t* undef_node()
{
    code_t* node = new_node(UNDEF_NODE);
    return node;
}

code_t* var_node(char* name)
{
    code_t* node = new_node(VAR_NODE);
    node->v.var = lookup_var(name);
    if (node->v.var == NULL) {
        yyerror("Undefined variable");
    }
    return node;
}

code_t* call_node(char* name, code_t* args)
{
    code_t* node = new_node(CALL_NODE);
    node->v.call.name = name;
    node->v.call.args = args;
    return node;
}

code_t* def_node(char* name, code_t* value)
{
    code_t* node = new_node(DEF_NODE);
    code_t* var = lookup_var(name);

    node->v.def.name = name;
    node->v.def.value = value;
   
    if (var != NULL && var->v.def.scope == in_subnet) {
        node->v.def.id = var->v.def.id;
        node->v.def.scope = var->v.def.scope;
    } else if (in_subnet) {
        node->v.def.id = subnet_num_ids++;
        node->v.def.scope = 1;
        add_var(name, node);
    } else {
        node->v.def.id = global_num_ids++;
        node->v.def.scope = 0;
        add_var(name, node);
    }

    return node;
}

code_t* param_node(char* name, code_t* value)
{
    code_t* node = def_node(name, value);
    node->tag = PARAM_NODE;
    return node;
}

code_t* process_node(char* name, code_t* defs, char* parent_name)
{
    code_t* node = new_node(PROCESS_NODE);
    node->v.process.name = name;
    node->v.process.defs = defs;
    if (parent_name != NULL) {
        node->v.process.parent = lookup_process(parent_name);
        if (node->v.process.parent == NULL) {
            yyerror("Invalid parent for process layer info");
        } else if (defs == NULL) {
	    node->v.process.defs = node->v.process.parent->v.process.defs;
	} else {
	    code_t* tail = defs;
	    while (tail->next != NULL)
	        tail = tail->next;
	    tail->next = node->v.process.parent->v.process.defs;
	}
    } else
        node->v.process.parent = NULL;
    add_process(name, node);
    return node;
}

code_t* name_node(char* name, code_t* indices)
{
    code_t* node = new_node(NAME_NODE);
    node->v.name.name = name;
    node->v.name.indices = indices;
    return node;
}

code_t* block_node(code_t* block)
{
    code_t* node = new_node(BLOCK_NODE);
    node->v.block = block;
    return node;
}

code_t* elt_node_head(char* basename, code_t* indices, char* model, 
                      code_t* process)
{
    code_t* node = new_node(ELEMENT_NODE);
    char name_buf[64];
    if (basename == NULL) {
        sprintf(name_buf, "anon%d", ++num_anon_elts);
        basename = mempool_strdup(parse_pool, name_buf);
    } 
    node->v.element.name = name_node(basename, indices);
    node->v.element.model = model;
    node->v.element.process = process;
    node->v.element.model_node = lookup_model(model);
    return node;
}

code_t* elt_node_finish(code_t* elt, code_t* nodes, code_t* params)
{
    elt->v.element.nodes = nodes;
    elt->v.element.params = params;
    if (elt->v.element.process == NULL) {
        elt->v.element.process = null_process;
        yyerror("Invalid process layer");
    }
    return elt;
}

code_t* for_node(char* var, code_t* lower, code_t* upper)
{
    code_t* node = new_node(FOR_NODE);
    node->v.forloop.idx = def_node(var, NULL);
    node->v.forloop.lower = lower;
    node->v.forloop.upper = upper;
    node->v.forloop.body = NULL;
    add_var(var, node->v.forloop.idx);
    return node;
}

code_t* for_node_body(code_t* node, code_t* body)
{
    node->v.forloop.body = body; 
    return node;
}

code_t* if_node(code_t* cond, code_t* then_clause, code_t* else_clause)
{
    code_t* node = new_node(IF_NODE);
    node->v.ifstmt.cond = cond;
    node->v.ifstmt.then_clause = then_clause;
    node->v.ifstmt.else_clause = else_clause;
    return node;
}

code_t* subnet_node(char* model, code_t* nodes, code_t* params)
{
    code_t* node = new_node(SUBNET_NODE);
    node->v.subnet.model = model;
    node->v.subnet.nodes = nodes;
    node->v.subnet.params = params;
    add_node_names(nodes);
    add_model(model, node);
    return node;
}

code_t* subnet_body(code_t* subnet, code_t* body)
{
    subnet->v.subnet.body = body;
    subnet->v.subnet.def_count = subnet_num_ids;
    return subnet;
}

