#include <sugar.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <model_circuit.h>
#include <mesh.h>
#include <vars.h>
#include <assemble.h>
#include <netdraw.h>
#include <netout.h>

typedef struct resistor_t {
    element_t element;
    int nodes[2];
    int vars[2];
    double G;
} resistor_t;

typedef struct capacitor_t {
    element_t element;
    int nodes[2];
    int vars[2];
    double C;
} capacitor_t;

typedef struct inductor_t {
    element_t element;
    int nodes[2];
    int vars[3];
    double L;
} inductor_t;

typedef struct isrc_t {
    element_t element;
    int nodes[2];
    int vars[2];
    double I;
} isrc_t;

typedef struct vsrc_t {
    element_t element;
    int nodes[2];
    int vars[3];
    double V;
} vsrc_t;

typedef struct eground_t {
    element_t element;
    int node;
    int var;
} eground_t;

static element_t* resistor_init(mesh_t mesh, const char* model,
                                        model_element_t* modelfunc)
{
    resistor_t* self = (resistor_t*) 
                       mempool_cget(mesh_pool(mesh), sizeof(*self));
    material_t* material;
    mesh_param_t* param;

    self->element.data = self;
    self->element.model = modelfunc;

    mesh_param_get_nodes(mesh, self->nodes, 2);
    material = mesh_param_get_material(mesh, "material");

    if ((param = mesh_param_byname(mesh, material, "G")) != NULL) {
        self->G = mesh_param_number(mesh, param);
    } else if ((param = mesh_param_byname(mesh, material, "R")) != NULL) {
        self->G = 1 / mesh_param_number(mesh, param);
    } else {
        mesh_error(mesh, "Either conductivity or resistivity must be defined");
    }

    return &(self->element);
}

static void resistor_vars(void* pself, vars_mgr_t vars)
{
    resistor_t* self = (resistor_t*) pself;

    self->vars[0] = vars_node(vars, self->nodes[0], "e");
    self->vars[1] = vars_node(vars, self->nodes[1], "e");
}

static void resistor_R(void* pself, assemble_matrix_t *R, 
                               assemble_matrix_t* x,
                               assemble_matrix_t* v,
                               assemble_matrix_t* a)
{
    if (x) {
        resistor_t* self = (resistor_t*) pself;
        double xlocal[2];
        double Rlocal[2];

        assemble_matrix_add(x, self->vars, 2, xlocal);
        Rlocal[0] =  self->G * (xlocal[0] - xlocal[1]);
        Rlocal[1] = -Rlocal[0];
        assemble_matrix_add(R, self->vars, 2, Rlocal);
    }
}

static void resistor_dR(void* pself, assemble_matrix_t* dR, 
                                double cx, assemble_matrix_t* x,
                                double cv, assemble_matrix_t* v,
                                double ca, assemble_matrix_t* a)
{
    if (cx) {
        resistor_t* self = (resistor_t*) pself;
        double dRlocal[4];
        dRlocal[0] =  cx * self->G;
        dRlocal[1] = -cx * self->G;
        dRlocal[2] = -cx * self->G;
        dRlocal[3] =  cx * self->G;
        assemble_matrix_add(dR, self->vars, 2, dRlocal);
    }
}

static void resistor_output(void* pself, netout_t* netout)
{
    resistor_t* self = (resistor_t*) pself;

    netout_string        (netout, "model", "resistor");
    netout_int_matrix    (netout, "nodes", self->nodes, 1, 2);
    netout_int_matrix    (netout, "vars",  self->vars,  1, 2);
    netout_double        (netout, "G",     self->G);
}

static element_t* capacitor_init(mesh_t mesh, const char* model,
                                         model_element_t* modelfunc)
{
    capacitor_t* self = (capacitor_t*) 
                        mempool_cget(mesh_pool(mesh), sizeof(*self));

    self->element.data = self;
    self->element.model = modelfunc;

    mesh_param_get_nodes(mesh, self->nodes, 2);
    self->C = mesh_param_get_number(mesh, NULL, "C");

    return &(self->element);
}

static void capacitor_vars(void* pself, vars_mgr_t vars)
{
    capacitor_t* self = (capacitor_t*) pself;

    self->vars[0] = vars_node(vars, self->nodes[0], "e");
    self->vars[1] = vars_node(vars, self->nodes[1], "e");
}

static void capacitor_R(void* pself, assemble_matrix_t *R, 
                                assemble_matrix_t* x,
                                assemble_matrix_t* v,
                                assemble_matrix_t* a)
{
    capacitor_t* self = (capacitor_t*) pself;

    if (v) {
        double vlocal[2];
        double Rlocal[2];

        assemble_matrix_add(v, self->vars, 2, vlocal);
        Rlocal[0] = self->C * (vlocal[0] - vlocal[1]);
        Rlocal[1] = -Rlocal[0];
        assemble_matrix_add(R, self->vars, 2, Rlocal);
    }
}

static void capacitor_dR(void* pself, assemble_matrix_t* dR, 
                                 double cx, assemble_matrix_t* x,
                                 double cv, assemble_matrix_t* v,
                                 double ca, assemble_matrix_t* a)
{
    capacitor_t* self = (capacitor_t*) pself;

    if (cv) {
        double dRlocal[4];
        dRlocal[0] =  cv * self->C;
        dRlocal[1] = -cv * self->C;
        dRlocal[2] = -cv * self->C;
        dRlocal[3] =  cv * self->C;
        assemble_matrix_add(dR, self->vars, 2, dRlocal);
    }
}

static void capacitor_output(void* pself, netout_t* netout)
{
    capacitor_t* self = (capacitor_t*) pself;

    netout_string        (netout, "model", "capacitor");
    netout_int_matrix    (netout, "nodes", self->nodes, 1, 2);
    netout_int_matrix    (netout, "vars",  self->vars,  1, 2);
    netout_double        (netout, "C",     self->C);
}

static element_t* inductor_init(mesh_t mesh, const char* model,
                                        model_element_t* modelfunc)
{
    inductor_t* self = (inductor_t*) 
                       mempool_cget(mesh_pool(mesh), sizeof(*self));

    self->element.data = self;
    self->element.model = modelfunc;

    mesh_param_get_nodes(mesh, self->nodes, 2);
    self->L = mesh_param_get_number(mesh, NULL, "L");

    return &(self->element);
}

static void inductor_vars(void* pself, vars_mgr_t vars)
{
    inductor_t* self = (inductor_t*) pself;

    self->vars[0] = vars_node(vars, self->nodes[0], "e");
    self->vars[1] = vars_node(vars, self->nodes[1], "e");
    self->vars[2] = vars_branch(vars, "i");
}

static void inductor_R(void* pself, assemble_matrix_t *R, 
                               assemble_matrix_t* x,
                               assemble_matrix_t* v,
                               assemble_matrix_t* a)
{
    inductor_t* self = (inductor_t*) pself;

    if (x) {
        double xlocal[3];
        double Rlocal[3];

        assemble_matrix_add(x, self->vars, 3, xlocal);
        Rlocal[0] =  xlocal[0];
        Rlocal[1] = -xlocal[1];
        Rlocal[2] =  xlocal[0] - xlocal[1];
        assemble_matrix_add(R, self->vars, 3, Rlocal);
    }

    if (v) {
        double vlocal;
        double Rlocal;

        assemble_matrix_add(v, self->vars + 3, 1, &vlocal);
        Rlocal = -self->L * vlocal;
        assemble_matrix_add(v, self->vars + 3, 1, &Rlocal);
    }
}

static void inductor_dR(void* pself, assemble_matrix_t* dR, 
                                double cx, assemble_matrix_t* x,
                                double cv, assemble_matrix_t* v,
                                double ca, assemble_matrix_t* a)
{
    inductor_t* self = (inductor_t*) pself;

    if (cx) {
        double dRlocal[9];
        memset(dRlocal, 0, 9 * sizeof(double));
        dRlocal[2] =  cx;
        dRlocal[5] = -cx;
        dRlocal[6] =  cx;
        dRlocal[7] = -cx;
        assemble_matrix_add(dR, self->vars, 3, dRlocal);
    }

    if (cv) {
        double dRlocal = -cv * self->L;
        assemble_matrix_add(dR, self->vars + 3, 1, &dRlocal);
    }
}

static void inductor_output(void* pself, netout_t* netout)
{
    inductor_t* self = (inductor_t*) pself;

    netout_string        (netout, "model", "inductor");
    netout_int_matrix    (netout, "nodes", self->nodes, 1, 2);
    netout_int_matrix    (netout, "vars",  self->vars,  1, 3);
    netout_double        (netout, "L",     self->L);
}

static element_t* isrc_init(mesh_t mesh, const char* model,
                                    model_element_t* modelfunc)
{
    isrc_t* self = (isrc_t*) 
                   mempool_cget(mesh_pool(mesh), sizeof(*self));

    self->element.data = self;
    self->element.model = modelfunc;

    mesh_param_get_nodes(mesh, self->nodes, 2);
    self->I = mesh_param_get_number(mesh, NULL, "I");

    return &(self->element);
}

static void isrc_vars(void* pself, vars_mgr_t vars)
{
    isrc_t* self = (isrc_t*) pself;

    self->vars[0] = vars_node(vars, self->nodes[0], "e");
    self->vars[1] = vars_node(vars, self->nodes[1], "e");
}

static void isrc_R(void* pself, assemble_matrix_t *R, 
                           assemble_matrix_t* x,
                           assemble_matrix_t* v,
                           assemble_matrix_t* a)
{
    isrc_t* self = (isrc_t*) pself;
    double Rlocal[2];

    Rlocal[0] =  self->I;
    Rlocal[1] = -self->I;
    assemble_matrix_add(R, self->vars, 2, Rlocal);
}

static void isrc_output(void* pself, netout_t* netout)
{
    isrc_t* self = (isrc_t*) pself;

    netout_string        (netout, "model", "isrc");
    netout_int_matrix    (netout, "nodes", self->nodes, 1, 2);
    netout_int_matrix    (netout, "vars",  self->vars,  1, 2);
    netout_double        (netout, "I",     self->I);
}

static element_t* vsrc_init(mesh_t mesh, const char* model,
                                    model_element_t* modelfunc)
{
    vsrc_t* self = (vsrc_t*) 
                   mempool_cget(mesh_pool(mesh), sizeof(*self));

    self->element.data = self;
    self->element.model = modelfunc;

    mesh_param_get_nodes(mesh, self->nodes, 2);
    self->V = mesh_param_get_number(mesh, NULL, "V");

    return &(self->element);
}

static void vsrc_vars(void* pself, vars_mgr_t vars)
{
    vsrc_t* self = (vsrc_t*) pself;

    self->vars[0] = vars_node(vars, self->nodes[0], "e");
    self->vars[1] = vars_node(vars, self->nodes[1], "e");
    self->vars[2] = vars_branch(vars, "i");
}

static void vsrc_R(void* pself, assemble_matrix_t *R, 
                           assemble_matrix_t* x,
                           assemble_matrix_t* v,
                           assemble_matrix_t* a)
{
    vsrc_t* self = (vsrc_t*) pself;

    double Rlocal[2];
    double xlocal[2];

    Rlocal[0] =  0;
    Rlocal[1] =  0;
    Rlocal[2] = -self->V;

    if (x) {
        assemble_matrix_add(x, self->vars, 3, xlocal);
        Rlocal[0] +=  xlocal[0];
        Rlocal[1] += -xlocal[1];
        Rlocal[2] +=  xlocal[0] - xlocal[1];
    }
    assemble_matrix_add(R, self->vars, 3, Rlocal);
}

static void vsrc_dR(void* pself, assemble_matrix_t* dR, 
                            double cx, assemble_matrix_t* x,
                            double cv, assemble_matrix_t* v,
                            double ca, assemble_matrix_t* a)
{
    vsrc_t* self = (vsrc_t*) pself;

    if (cx) {
        double dRlocal[9];
        memset(dRlocal, 0, 9 * sizeof(double));
        dRlocal[2] =  cx;
        dRlocal[5] = -cx;
        dRlocal[6] =  cx;
        dRlocal[7] = -cx;
        assemble_matrix_add(dR, self->vars, 3, dRlocal);
    }
}

static void vsrc_output(void* pself, netout_t* netout)
{
    vsrc_t* self = (vsrc_t*) pself;

    netout_string        (netout, "model", "vsrc");
    netout_int_matrix    (netout, "nodes", self->nodes, 1, 2);
    netout_int_matrix    (netout, "vars",  self->vars,  1, 3);
    netout_double        (netout, "V",     self->V);
}

static element_t* eground_init(mesh_t mesh, const char* model,
                                       model_element_t* modelfunc)
{
    eground_t* self = (eground_t*) 
                   mempool_cget(mesh_pool(mesh), sizeof(*self));

    self->element.data = self;
    self->element.model = modelfunc;

    mesh_param_get_nodes(mesh, &(self->node), 1);

    return &(self->element);
}

static void eground_vars(void* pself, vars_mgr_t vars)
{
    eground_t* self = (eground_t*) pself;

    self->var = vars_node(vars, self->node, "e");
}

static void eground_displace(void* pself, assemble_matrix_t* assembler)
{
    eground_t* self = (eground_t*) pself;
    assemble_matrix_add1(assembler, self->var, 0);
}

static void eground_output(void* pself, netout_t* netout)
{
    eground_t* self = (eground_t*) pself;

    netout_string        (netout, "model", "eground");
    netout_int_matrix    (netout, "node", &(self->node), 1, 1);
    netout_int_matrix    (netout, "var",  &(self->var),  1, 1);
}

void model_circuit_register(model_mgr_t model_mgr)
{
    model_element_t model;

    memset(&model, 0, sizeof(model));
    model.init    = resistor_init;
    model.R       = resistor_R;
    model.dR      = resistor_dR;
    model.vars    = resistor_vars;
    model.output  = resistor_output;
    model_mgr_add_element(model_mgr, "resistor", &model);
    
    memset(&model, 0, sizeof(model));
    model.init    = capacitor_init;
    model.vars    = capacitor_vars;
    model.R       = capacitor_R;
    model.dR      = capacitor_dR;
    model.output  = capacitor_output;
    model_mgr_add_element(model_mgr, "capacitor", &model);
    
    memset(&model, 0, sizeof(model));
    model.init    = inductor_init;
    model.vars    = inductor_vars;
    model.R       = inductor_R;
    model.dR      = inductor_dR;
    model.output  = inductor_output;
    model_mgr_add_element(model_mgr, "inductor", &model);
    
    memset(&model, 0, sizeof(model));
    model.init    = isrc_init;
    model.vars    = isrc_vars;
    model.R       = isrc_R;
    model.output  = isrc_output;
    model_mgr_add_element(model_mgr, "isrc", &model);
    
    memset(&model, 0, sizeof(model));
    model.init    = vsrc_init;
    model.vars    = vsrc_vars;
    model.R       = vsrc_R;
    model.dR      = vsrc_dR;
    model.output  = vsrc_output;
    model_mgr_add_element(model_mgr, "vsrc", &model);
    
    memset(&model, 0, sizeof(model));
    model.init     = eground_init;
    model.vars     = eground_vars;
    model.displace = eground_displace;
    model.output   = eground_output;
    model_mgr_add_element(model_mgr, "eground", &model);
    
}
