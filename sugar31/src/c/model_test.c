#include <sugar.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <model_test.h>
#include <modelmgr.h>
#include <mesh.h>
#include <netout.h>

typedef struct model_test_element_t {
    element_t element;
    const char* model;
    int* nodes;
    int num_nodes;
    mesh_param_t* params;
    int num_params;
} model_test_element_t;

typedef struct model_test_material_t {
    material_t material;
    const char* model;
    mesh_param_t* params;
    int num_params;
    material_t* parent;
} model_test_material_t;

static void print_nodes(model_test_element_t* self, netout_t* netout)
{
    netout_int_matrix(netout, "nodes", self->nodes, 1, self->num_nodes);
}

static void print_params(int num_params, mesh_param_t* params, 
                                 netout_t* netout)
{
    int i;

    for (i = 0; i < num_params; ++i) {
        if (params[i].tag == MESH_PARAM_STRING)
            netout_string(netout, params[i].name, params[i].val.s);
        else if (params[i].tag == MESH_PARAM_NUMBER)
            netout_double(netout, params[i].name, params[i].val.d);
        else if (params[i].tag == MESH_PARAM_MATRIX)
            netout_double_matrix(netout, params[i].name,
                                 params[i].val.m.data,
                                 params[i].val.m.m,
                                 params[i].val.m.n);
        else
            netout_string(netout, params[i].name, "???");
    }
}

static element_t* test_element_init(mesh_t mesh, const char* name,
                                            model_element_t* modelfunc)
{
    int i;
    mempool_t pool = mesh_pool(mesh);
    model_test_element_t* self = (model_test_element_t*)
        mempool_cget(pool, sizeof(*self));

    self->element.data = self;
    self->element.model = modelfunc;

    self->model = name;
    self->num_nodes = mesh_num_param_nodes(mesh);
    self->nodes = (int*) 
        mempool_cget(pool, self->num_nodes * sizeof(int));
    for (i = 0; i < self->num_nodes; ++i)
        self->nodes[i] = mesh_param_node(mesh, i);
    self->num_params = mesh_num_params(mesh);
    self->params = (mesh_param_t*)
        mempool_cget(pool, self->num_params * sizeof(mesh_param_t));
    for (i = 0; i < self->num_params; ++i)
        self->params[i] = *mesh_param(mesh, i);

    return &(self->element);
}

static void output_element(void* pself, netout_t* netout)
{
    model_test_element_t* self = (model_test_element_t*) pself;
    netout_string(netout, "model", self->model);
    print_nodes(self, netout);
    print_params(self->num_params, self->params, netout);
}

static material_t* test_material_init(mesh_t mesh, const char* name,
                                              model_material_t* modelfunc)
{
    int i;
    mempool_t pool = mesh_pool(mesh);

    model_test_material_t* self = (model_test_material_t*)
        mempool_cget(pool, sizeof(*self));
    mesh_param_t* parent_param = NULL;
    self->material.model = modelfunc;
    self->material.data = self;

    self->model = name;
    self->num_params = mesh_num_params(mesh);
    self->params = (mesh_param_t*)
        mempool_cget(pool, self->num_params * sizeof(mesh_param_t));
    for (i = 0; i < self->num_params; ++i)
        self->params[i] = *mesh_param(mesh, i);
    parent_param = mesh_param_byname(mesh, NULL, "parent");
    if (parent_param)
        self->parent = mesh_param_material(mesh, parent_param);

    return &(self->material);
}

static struct mesh_param_t* 
test_material_param(void* pself, const char* name)
{
    model_test_material_t* self = (model_test_material_t*) pself;
    int i;

    for (i = 0; i < self->num_params; ++i) {
        if (strcmp(name, self->params[i].name) == 0)
            return &(self->params[i]);
    }
    if (self->parent != NULL && self->parent->model->param != NULL)
        return (self->parent->model->param)(self->parent->data, name);
    return NULL;
}

static void output_material(void* pself, netout_t* netout)
{
    model_test_material_t* self = (model_test_material_t*) pself;
    netout_string(netout, "model", self->model);
    print_params(self->num_params, self->params, netout);
}

void model_test_register(model_mgr_t model_mgr)
{
    model_element_t  element;
    model_material_t material;

    memset(&element,  0, sizeof(element));
    memset(&material, 0, sizeof(material));

    element.init   = test_element_init;
    element.output = output_element;

    material.init   = test_material_init;
    material.param  = test_material_param;
    material.output = output_material;

    model_mgr_add_element(model_mgr, "test", &element);
    model_mgr_add_material(model_mgr, NULL, &material);
}

