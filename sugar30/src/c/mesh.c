#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "mempool.h"
#include "hash.h"
#include "mesh.h"
#include "vars.h"
#include "assemble.h"
#include "dynarray.h"

struct mesh_struct {
    dynarray_t nodes;
    dynarray_t elements;
    dynarray_t materials;
    dynarray_t param_nodes;
    dynarray_t params;
    mempool_t pool;
    hash_t strings;
    model_mgr_t model_mgr;
    void (*error_handler)(void* arg, const char* msg);
    void (*warning_handler)(void* arg, const char* msg);
    void* handler_arg;
    vars_mgr_t vars_mgr;
    assembler_t assembler;
};

#define NODES_SIZ      128
#define ELEMENT_SIZ    128
#define MATERIAL_SIZ   16
#define ELT_NODES_SIZ  27
#define PARAMS_SIZ     64
#define STRINGS_SIZ    500

static void default_error_handler(void* arg, const char* msg)
{
    fprintf(stderr, "%s\n", msg);
    abort();
}

static void default_warning_handler(void* arg, const char* msg)
{
    fprintf(stderr, "%s\n", msg);
}

vars_mgr_t mesh_vars_mgr(mesh_t self)
{
    return self->vars_mgr;
}

assembler_t mesh_assembler(mesh_t self)
{
    return self->assembler;
}

mesh_t mesh_create(model_mgr_t model_mgr)
{
    mempool_t pool = mempool_create(MEMPOOL_DEFAULT_SPAN);
    mesh_t    self = (mesh_t) mempool_get(pool, sizeof(struct mesh_struct));

    self->nodes     = dynarray_create(sizeof(mesh_node_t), NODES_SIZ);
    self->elements  = dynarray_create(sizeof(element_t*),  ELEMENT_SIZ);
    self->materials = dynarray_create(sizeof(material_t*), MATERIAL_SIZ);

    self->param_nodes = dynarray_create(sizeof(int), ELT_NODES_SIZ);
    self->params = dynarray_create(sizeof(mesh_param_t), PARAMS_SIZ);

    self->pool      = pool;
    self->strings   = hstrings_pcreate(pool, STRINGS_SIZ);
    self->model_mgr = model_mgr;

    mesh_set_handlers(self, NULL, NULL, NULL);

    self->vars_mgr  = vars_create(self);
    self->assembler = assemble_create(self);

    return self;
}

void mesh_destroy(mesh_t self)
{
    int i, n;

    assemble_destroy(self->assembler);
    vars_destroy(self->vars_mgr);

    n = mesh_num_elements(self);
    for (i = 1; i <= n; ++i)
        element_destroy( mesh_element(self, i) );

    n = mesh_num_materials(self);
    for (i = 1; i <= n; ++i)
        material_destroy( mesh_material(self, i) );

    dynarray_destroy(self->params);
    dynarray_destroy(self->param_nodes);

    dynarray_destroy(self->materials);
    dynarray_destroy(self->elements);
    dynarray_destroy(self->nodes);

    mempool_destroy(self->pool);
}

mempool_t mesh_pool(mesh_t self)
{
    return self->pool;
}

void mesh_set_handlers(mesh_t self,
                       void (*error_handler)(void* arg, const char* msg),
                       void (*warning_handler)(void* arg, const char* msg),
                       void* arg)
{
    self->error_handler = 
        (error_handler == NULL) ? default_error_handler : error_handler;
    self->warning_handler = 
        (error_handler == NULL) ? default_warning_handler : warning_handler;
    self->handler_arg = arg;
}

void mesh_error(mesh_t self, const char* msg)
{
    (self->error_handler)(self->handler_arg, msg);
}

void mesh_warning(mesh_t self, const char* msg)
{
    (self->warning_handler)(self->handler_arg, msg);
}

void mesh_add_param_number(mesh_t self, const char* name, double d)
{
    mesh_param_t param;
    param.name  = hstrings_add(self->strings, name);
    param.tag   = MESH_PARAM_NUMBER;
    param.val.d = d;
    dynarray_append(self->params, &param);
}

void mesh_add_param_string(mesh_t self, const char* name, const char* s)
{
    mesh_param_t param;
    param.name  = hstrings_add(self->strings, name);
    param.tag   = MESH_PARAM_STRING;
    param.val.s = hstrings_add(self->strings, s);
    dynarray_append(self->params, &param);
}

void mesh_add_param_matrix(mesh_t self, const char* name, int m, int n,
                           double* data)
{
    mesh_param_t param;
    param.name = hstrings_add(self->strings, name);
    param.tag  = MESH_PARAM_MATRIX;
    param.val.m.m = m;
    param.val.m.n = n;
    param.val.m.data = mempool_memdup(self->pool, data, m*n * sizeof(double));
    dynarray_append(self->params, &param);
}

void mesh_add_param_node(mesh_t self, int node_id)
{
    if (node_id <= 0 || node_id > mesh_num_nodes(self))
        mesh_error(self, "Invalid node identifier");
    dynarray_append(self->param_nodes, &node_id);
}

mesh_param_t* mesh_param(mesh_t self, int index)
{
    return dynarray_get(self->params, index);
}

int mesh_param_node(mesh_t self, int index)
{
    int* n = (int*) dynarray_get(self->param_nodes, index);
    return (n == NULL) ? 0 : *n;
}

mesh_param_t* mesh_param_byname(mesh_t self, const char* name)
{
    mesh_param_t* params = (mesh_param_t*) dynarray_data(self->params);
    int n = dynarray_count(self->params);
    int i;
    for (i = 0; i < n; ++i) {
        if (strcmp(params[i].name, name) == 0)
            return &(params[i]);
    }
    return NULL;
}

int mesh_num_params(mesh_t self)
{
    return dynarray_count(self->params);
}

int mesh_num_param_nodes(mesh_t self)
{
    return dynarray_count(self->param_nodes);
}

void mesh_params_clear(mesh_t self)
{
    dynarray_set_count(self->params, 0);
    dynarray_set_count(self->param_nodes, 0);
}

const char* mesh_param_string(mesh_t self, mesh_param_t* param)
{
    if (param == NULL) {
        mesh_error(self, "Missing parameter");
        return NULL;
    } else if (param->tag != MESH_PARAM_STRING) {
        mesh_error(self, "Parameter should be a string");
        return NULL;
    } else
        return param->val.s;
}

double mesh_param_number(mesh_t self, mesh_param_t* param)
{
    if (param == NULL) {
        mesh_error(self, "Missing parameter");
        return 0;
    } else if (param->tag != MESH_PARAM_NUMBER) {
        mesh_error(self, "Parameter should be numeric");
        return 0;
    } else 
        return param->val.d;
}

int mesh_param_int(mesh_t self, mesh_param_t* param)
{
    double v = mesh_param_number(self, param);
    if (v != (int) v)
        mesh_error(self, "Number is not an integer");
    return (int) v;
}

material_t* mesh_param_material(mesh_t self, mesh_param_t* param)
{
    int material_id = mesh_param_int(self, param);
    if (material_id <= 0 || material_id > mesh_num_materials(self)) {
        mesh_error(self, "Invalid material identifier");
        return NULL;
    } else
        return mesh_material(self, material_id);
}

int mesh_add_node(mesh_t self, const char* name, double x, double y, double z)
{
    mesh_node_t node;
    node.name = hstrings_add(self->strings, name);
    node.x[0] = x;
    node.x[1] = y;
    node.x[2] = z;
    dynarray_append(self->nodes, &node);
    return dynarray_count(self->nodes);
}

int mesh_add_element(mesh_t self, const char* model)
{
    element_t* element;
    model = hstrings_add(self->strings, model);
    element = element_init(self->model_mgr, self, model);
    dynarray_set_count(self->params, 0);
    dynarray_set_count(self->param_nodes, 0);
    dynarray_append(self->elements, &element);
    return dynarray_count(self->elements);
}

int mesh_add_material(mesh_t self, const char* model)
{
    material_t* material;
    model = hstrings_add(self->strings, model);
    material = material_init(self->model_mgr, self, model);
    dynarray_set_count(self->params, 0);
    dynarray_append(self->materials, &material);
    return dynarray_count(self->materials);
}

mesh_node_t* mesh_node(mesh_t self, int node_id)
{
    return dynarray_get(self->nodes, node_id-1);
}

element_t* mesh_element(mesh_t self, int element_id)
{
    return *(element_t**) dynarray_get(self->elements, element_id-1);
}

material_t* mesh_material(mesh_t self, int material_id)
{
    return *(material_t**) dynarray_get(self->materials, material_id-1);
}

int mesh_num_nodes(mesh_t self)
{
    return dynarray_count(self->nodes);
}

int mesh_num_elements(mesh_t self)
{
    return dynarray_count(self->elements);
}

int mesh_num_materials(mesh_t self)
{
    return dynarray_count(self->materials);
}

