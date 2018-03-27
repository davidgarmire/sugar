#include <sugar.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <model_force.h>
#include <mesh.h>
#include <vars.h>
#include <assemble.h>
#include <netout.h>

typedef struct bc_t {
    element_t     element;
    const char*   model;
    int           node;
    int*          vars;
    const char**  var_names;
    double*       var_values;
    int           num_vars;
} bc_t;

static element_t* bc_init(mesh_t mesh, const char* model, 
                                  model_element_t* modelfunc)
{
    int i, j;
    mempool_t pool = mesh_pool(mesh);
    bc_t*     self = (bc_t*) mempool_cget(pool, sizeof(bc_t));

    self->element.data = self;
    self->element.model = modelfunc;

    self->model = model;
    mesh_param_get_nodes(mesh, &(self->node), 1);
    self->num_vars   = mesh_num_params(mesh);
    self->vars       = mempool_cget( pool, self->num_vars * sizeof(int)    );
    self->var_names  = mempool_cget( pool, self->num_vars * sizeof(char*)  );
    self->var_values = mempool_cget( pool, self->num_vars * sizeof(double) );
    
    j = 0;
    for (i = 0; i < self->num_vars; ++i) {
        mesh_param_t* param = mesh_param(mesh, i);
        if (strcmp(param->name, "model") != 0) {
            self->var_names[j]  = param->name;
            self->var_values[j] = mesh_param_number(mesh, param);
            ++j;
        }
    }
    
    self->num_vars = j;
    

    return &(self->element);
}

static void bc_vars(void* pself, vars_mgr_t vars)
{
    int i;
    bc_t* self = (bc_t*) pself;

    for (i = 0; i < self->num_vars; ++i)
        self->vars[i] = vars_node(vars, self->node, self->var_names[i]);
}

static void bc_displace(void* pself, assemble_matrix_t* assembler)
{
    bc_t* self = (bc_t*) pself;

    assemble_matrix_add(assembler, self->vars, self->num_vars, 
                        self->var_values);
}

static void bc_R(void* pself, assemble_matrix_t* R, 
                         assemble_matrix_t* x,
                         assemble_matrix_t* v,
                         assemble_matrix_t* a)
{
    int i;
    bc_t* self = (bc_t*) pself;

    for (i = 0; i < self->num_vars; ++i)
        assemble_matrix_add(R, self->vars, self->num_vars, 
                            self->var_values);
}

static void bc_output(void* pself, netout_t* netout)
{
    int i;
    bc_t* self = (bc_t*) pself;

    netout_string     (netout, "model", self->model);
    netout_int_matrix (netout, "node",  &(self->node), 1, 1);
    netout_int_matrix (netout, "vars",  self->vars, 1, self->num_vars);

    for (i = 0; i < self->num_vars; ++i)
        netout_double(netout, self->var_names[i], self->var_values[i]);
}

void model_force_register(model_mgr_t model_mgr)
{
    model_element_t model;

    memset(&model, 0, sizeof(model));
    model.init     = bc_init;
    model.vars     = bc_vars;
    model.R        = bc_R;
    model.output   = bc_output;
    model_mgr_add_element(model_mgr, "force", &model);

    memset(&model, 0, sizeof(model));
    model.init     = bc_init;
    model.vars     = bc_vars;
    model.displace = bc_displace;
    model.output   = bc_output;
    model_mgr_add_element(model_mgr, "displace", &model);
}

