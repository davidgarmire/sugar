#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "model-anchor.h"
#include "mesh.h"
#include "vars.h"
#include "assemble.h"
#include "netout.h"

typedef struct anchor_t {
    element_t element;
    int node;
    int vars[3];
    double l;
    double w;
    double h;
} anchor_t;

static double get_num_param(mesh_t mesh,
                            material_t* material,
                            const char* name)
{
    mesh_param_t* param = mesh_param_byname(mesh, name);
    if (param == NULL && material != NULL)
        param = material_param(material, name);
    if (param == NULL)
        return 0;
    return mesh_param_number(mesh, param);
}

static element_t* anchor_init(mesh_t mesh, const char* model,
                              model_element_t* modelfunc)
{
    anchor_t* self = (anchor_t*) 
        mempool_cget(mesh_pool(mesh), sizeof(anchor_t));
    material_t* material = NULL;

    self->element.model = modelfunc;
    self->element.data = self;

    material = mesh_param_material(mesh, mesh_param_byname(mesh, "material"));
    if (mesh_num_param_nodes(mesh) != 1)
        mesh_error(mesh, "Incorrect number of nodes for anchor");
    self->node = mesh_param_node(mesh, 0);

    self->l = get_num_param(mesh, material, "l");
    self->w = get_num_param(mesh, material, "w");
    self->h = get_num_param(mesh, material, "h");

    return &(self->element);
}

static void anchor_vars(void* pself, vars_mgr_t vars)
{
    anchor_t* self = (anchor_t*) pself;
    self->vars[0] = vars_node(vars, self->node, "x");
    self->vars[1] = vars_node(vars, self->node, "y");
    self->vars[2] = vars_node(vars, self->node, "rz");
}

static void anchor_displace(void* pself, assemble_matrix_t* assembler)
{
    anchor_t* self = (anchor_t*) pself;
    assemble_matrix_add1(assembler, self->vars[0], 0);
    assemble_matrix_add1(assembler, self->vars[1], 0);
    assemble_matrix_add1(assembler, self->vars[2], 0);
}

static void anchor_output(void* pself, netout_t* netout)
{
    anchor_t* self = (anchor_t*) pself;

    netout_string     (netout, "model", "anchor");
    netout_int_matrix (netout, "node",  &(self->node), 1, 1);
    netout_double     (netout, "l",     self->l);
    netout_double     (netout, "w",     self->w);
    netout_double     (netout, "h",     self->h);
    netout_int_matrix (netout, "vars",  self->vars, 1, 3);
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

