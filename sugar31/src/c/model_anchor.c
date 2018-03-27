#include <sugar.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <model_anchor.h>
#include <mesh.h>
#include <vars.h>
#include <assemble.h>
#include <netout.h>

typedef struct anchor_t {
    element_t element;
    int node;
    int vars[6];
    double l;
    double w;
    double h;
} anchor_t;

static element_t* anchor_init(mesh_t mesh, const char* model,
                                      model_element_t* modelfunc)
{
    anchor_t* self = (anchor_t*) 
        mempool_cget(mesh_pool(mesh), sizeof(anchor_t));
    material_t* material = NULL;

    self->element.model = modelfunc;
    self->element.data = self;

    mesh_param_get_nodes(mesh, &(self->node), 1);
    material = mesh_param_get_material(mesh, "material");
    self->l  = mesh_param_get_number(mesh, material, "l");
    self->w  = mesh_param_get_number(mesh, material, "w");
    self->h  = mesh_param_get_number(mesh, material, "h");

    return &(self->element);
}

static void anchor_vars(void* pself, vars_mgr_t vars)
{
    anchor_t* self = (anchor_t*) pself;
    self->vars[0] = vars_node(vars, self->node, "x");
    self->vars[1] = vars_node(vars, self->node, "y");
    self->vars[2] = vars_node(vars, self->node, "z");
    self->vars[3] = vars_node(vars, self->node, "rx");
    self->vars[4] = vars_node(vars, self->node, "ry");
    self->vars[5] = vars_node(vars, self->node, "rz");
}

static void anchor_displace(void* pself, assemble_matrix_t* assembler)
{
    anchor_t* self = (anchor_t*) pself;
    assemble_matrix_add1(assembler, self->vars[0], 0);
    assemble_matrix_add1(assembler, self->vars[1], 0);
    assemble_matrix_add1(assembler, self->vars[2], 0);
    assemble_matrix_add1(assembler, self->vars[3], 0);
    assemble_matrix_add1(assembler, self->vars[4], 0);
    assemble_matrix_add1(assembler, self->vars[5], 0);
}

static void anchor_output(void* pself, netout_t* netout)
{
    anchor_t* self = (anchor_t*) pself;

    netout_string     (netout, "model", "anchor");
    netout_int_matrix (netout, "node",  &(self->node), 1, 1);
    netout_double     (netout, "l",     self->l);
    netout_double     (netout, "w",     self->w);
    netout_double     (netout, "h",     self->h);
    netout_int_matrix (netout, "vars",  self->vars, 1, 6);
}

void model_anchor_register(model_mgr_t model_mgr)
{
    model_element_t model;
    memset(&model, 0, sizeof(model));

    model.init     = anchor_init;
    model.vars     = anchor_vars;
    model.displace = anchor_displace;
    model.output   = anchor_output;

    model_mgr_add_element(model_mgr, "anchor", &model);
}

