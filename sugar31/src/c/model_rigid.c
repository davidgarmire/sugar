#include <sugar.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <model_rigid.h>
#include <affine.h>
#include <mesh.h>
#include <vars.h>
#include <assemble.h>
#include <netdraw.h>
#include <netout.h>

typedef struct rigid_t {
    element_t element;
    int node;
    int vars[3];
    double m;
    double J;
} rigid_t;

typedef struct constraint2d_t {
    element_t element;
    int node[2];
    int vars[9];
    double relpos[3];
} constraint2d_t;

static element_t* rigid_init(mesh_t mesh, const char* model,
                                     model_element_t* modelfunc)
{
    rigid_t* self = (rigid_t*) 
        mempool_cget(mesh_pool(mesh), sizeof(*self));

    self->element.data = self;
    self->element.model = modelfunc;

    mesh_param_get_nodes(mesh, &(self->node), 1);

    return &(self->element);
}

static void rigid_vars(void* pself, vars_mgr_t vars)
{
    rigid_t* self = (rigid_t*) pself;

    self->vars[0] = vars_node(vars, self->node, "x");
    self->vars[1] = vars_node(vars, self->node, "y");
    self->vars[2] = vars_node(vars, self->node, "rz");
}

static void rigid_output(void* pself, netout_t* netout)
{
    rigid_t* self = (rigid_t*) pself;

    netout_string        (netout, "model",     "rigid");
    netout_int           (netout, "node",      self->node);
    netout_int_matrix    (netout, "vars",      self->vars, 1, 3);
}

static element_t* constraint2d_init(mesh_t mesh, const char* model,
                                          model_element_t* modelfunc)
{
    constraint2d_t* self = (constraint2d_t*) 
        mempool_cget(mesh_pool(mesh), sizeof(*self));

    self->element.data = self;
    self->element.model = modelfunc;

    mesh_param_get_nodes(mesh, self->node, 2);

    return &(self->element);
}

static void constraint2d_set_position(void* pself, mesh_t mesh)
{
    constraint2d_t* self = (constraint2d_t*) pself;
    double* x1 = mesh_node(mesh, self->node[0])->x;
    double* x2 = mesh_node(mesh, self->node[1])->x;

    self->relpos[0] = x2[0] - x1[0];
    self->relpos[1] = x2[1] - x1[1];
    self->relpos[2] = x2[2] - x1[2];
}

static void constraint2d_vars(void* pself, vars_mgr_t vars)
{
    constraint2d_t* self = (constraint2d_t*) pself;

    self->vars[0] = vars_node(vars, self->node[0], "x");
    self->vars[1] = vars_node(vars, self->node[0], "y");
    self->vars[2] = vars_node(vars, self->node[0], "rz");

    self->vars[3] = vars_node(vars, self->node[1], "x");
    self->vars[4] = vars_node(vars, self->node[1], "y");
    self->vars[5] = vars_node(vars, self->node[1], "rz");

    /* Multiplier variables */
    self->vars[6] = vars_branch(vars, "lx" );
    self->vars[7] = vars_branch(vars, "ly" );
    self->vars[8] = vars_branch(vars, "lrz");
}

static void constraint2d_R(void* pself, assemble_matrix_t *R, 
                                 assemble_matrix_t* x,
                                 assemble_matrix_t* v,
                                 assemble_matrix_t* a)
{
    constraint2d_t* self = (constraint2d_t*) pself;

    #define Ri(i) Rlocal[(i)-1]
    double Rlocal[9];

    #define xi(i) xlocal[(i)-1]
    double xlocal[9];
    
    double* relpos = self->relpos;
    double  c, s;
    
    assemble_matrix_add(x, self->vars, 9, xlocal);
    c = cos(xi(6));
    s = sin(xi(6));
    memset(Rlocal, 0, sizeof(Rlocal));

    Ri(4) = -xi(7);
    Ri(5) = -xi(8);
    Ri(6) = -xi(9);
    Ri(1) = xi(7);
    Ri(2) = xi(8);
    Ri(3) = (-s * relpos[0]  +  -c * relpos[1]) * xi(7) +
            (-s * relpos[0]  +  -c * relpos[1]) * xi(8) + 
            xi(9);
    Ri(7) = xi(1) - xi(4) + (( c * relpos[0]  +  -s * relpos[1]) - relpos[0]);
    Ri(8) = xi(2) - xi(5) + (( s * relpos[0]  +   c * relpos[1]) - relpos[1]);
    Ri(9) = xi(3) - xi(6);

    #undef xi
    #undef Ri

    assemble_matrix_add(R, self->vars, 9, Rlocal);
}

static void constraint2d_dR(void* pself, assemble_matrix_t* dR, 
                                  double cx, assemble_matrix_t* x,
                                  double cv, assemble_matrix_t* v,
                                  double ca, assemble_matrix_t* a)
{
    constraint2d_t* self = (constraint2d_t*) pself;

    #define Kij(i,j) Klocal[(i) + (j)*9 - 10]
    double Klocal[81];

    #define xi(i) xlocal[(i)-1]
    double xlocal[9];
    
    double* relpos = self->relpos;
    double  c, s;
    
    assemble_matrix_add(x, self->vars, 9, xlocal);
    c = cos(xi(6));
    s = sin(xi(6));
    memset(Klocal, 0, sizeof(Klocal));

    Kij(7,4) = Kij(4,7) = -1;
    Kij(8,5) = Kij(5,8) = -1;
    Kij(9,6) = Kij(6,9) = -1;
    Kij(7,1) = Kij(1,7) =  1;
    Kij(8,2) = Kij(2,8) =  1;
    
    Kij(7,3) = Kij(3,7) = (-s * relpos[0]  +  -c * relpos[1]);
    Kij(8,3) = Kij(3,8) = ( c * relpos[0]  +  -s * relpos[1]);
    Kij(9,3) = Kij(3,9) =  1;
    

    #undef xi
    #undef Kij

    assemble_matrix_add(dR, self->vars, 9, Klocal);
}

static void constraint2d_output(void* pself, netout_t* netout)
{
    constraint2d_t* self = (constraint2d_t*) pself;

    netout_string        (netout, "model",     "constraint2d");
    netout_int_matrix    (netout, "node",      self->node, 1, 2);
    netout_int_matrix    (netout, "vars",      self->vars, 1, 9);
}

void model_rigid_register(model_mgr_t model_mgr)
{
    model_element_t model;

    memset(&model, 0, sizeof(model));
    model.init    = rigid_init;
    model.vars    = rigid_vars;
    model.output  = rigid_output;
    model_mgr_add_element(model_mgr, "rigid", &model);
    
    memset(&model, 0, sizeof(model));
    model.init         = constraint2d_init;
    model.set_position = constraint2d_set_position;
    model.vars         = constraint2d_vars;
    model.R            = constraint2d_R;
    model.dR           = constraint2d_dR;
    model.output       = constraint2d_output;
    model_mgr_add_element(model_mgr, "constraint",   &model);
    model_mgr_add_element(model_mgr, "constraint2d", &model);
    
}
