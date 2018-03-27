#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "model-gap.h"
#include "affine.h"
#include "mesh.h"
#include "vars.h"
#include "assemble.h"
#include "netdraw.h"
#include "netout.h"

typedef struct gap_t {
    element_t element;
    int node[2];
    int vars[6];
    double relpos[3];
    double g0;
    double l, w, h;
    double V;
} gap_t;


static void compute_forces(double* F, double* dF,
                           double Ke1, double Ke2, 
                           double a, double t, double L)
{
    double z = (t*L)/(2*a);
    double s = 1/((1+z)*(1-z));

    double p1a, p1aa, p1at, p1t, p1tt;
    double p2a, p2aa, p2at, p2t, p2tt;

    double a2 = a*a, a3 = a2*a;
    double s2 = s*s;
    double z2 = z*z, z3 = z2*z;
    double L2 = L*L, L3 = L2*L;

    p1a  =   -L  / a2 * s;
    p1aa =  2*L  / a3 * s2;
    p1at =   -L2 / a3 * s2*z;

    if (abs(z) > 0.1) {

        double r = log((1+z)/(1-z));

        p1t  =  L2 / (4*a2*z2) * (2*z*s - r);
        p1tt =  L3 / (4*a3*z3) * (r + (4*z2 - 2)*z*s2);
        p2a  =   L / (2*a*z) * r;
        p2t  =  L2 / (4*a*z2) * (2*z - r);
        p2tt =  L3 / (4*a2*z3) * (r - (2-z2)*z*s);

    } else {
        double z2 = z*z;

        p1t = L2*z / (2*a2) *
        (2/3 + z2*(4/5 + z2*(5/7 + z2*(7/9 + z2*(9/11 + z2*(11/13))))));

        p1tt = L3 / (4*a3) *
        (2/3 + z2*(12/5 + z2*(20/7 + z2*(42/9 + z2*(72/11 + z2*(110/13))))));

        p2a = L/a *
        (1 + z2*(1/3 + z2*(1/5 + z2*(1/7 + z2*(1/9 + z2*(1/11))))));

        p2t = -L2*z/(2*a) * 
        (1/3 + z2*(1/5 + z2*(1/7 + z2*(1/9 + z2*(1/11 + z2*(1/13))))));

        p2tt = -L3/(4*a2) *
        (1/3 + z2*(3/5 + z2*(5/7 + z2*(7/9 + z2*(9/11 + z2*(11/13))))));

    }

    p2aa = p1a;
    p2at = p1t;

    F[0]  = -Ke1*p1a  + Ke2*p2a;
    F[1]  = -Ke1*p1t  + Ke2*p2t;

    dF[0] = -Ke1*p1aa + Ke2*p2aa;
    dF[3] = -Ke1*p1tt + Ke2*p2tt;
    dF[1] = -Ke1*p1at + Ke2*p2at;
    dF[2] = dF[1];
}

static double get_number(mesh_t mesh, material_t* material,
                         const char* name)
{
    mesh_param_t* param = mesh_param_byname(mesh, name);
    if (param == NULL && material != NULL && material->model != NULL &&
            material->model->param != NULL)
        param = (material->model->param)(material->data, name);
    return mesh_param_number(mesh, param);    
}

static element_t* gap_init(mesh_t mesh, const char* model,
                           model_element_t* modelfunc)
{
    gap_t* self = (gap_t*) mempool_cget(mesh_pool(mesh), sizeof(*self));
    material_t* material;

    self->element.data = self;
    self->element.model = modelfunc;

    if (mesh_num_param_nodes(mesh) != 2)
        mesh_error(mesh, "Incorrect number of nodes for gap");
    self->node[0] = mesh_param_node(mesh, 0);
    self->node[1] = mesh_param_node(mesh, 1);

    material = mesh_param_material(mesh, mesh_param_byname(mesh, "material"));
    
    self->l = get_number(mesh, material, "l");
    self->w = get_number(mesh, material, "w");
    self->h = get_number(mesh, material, "h");
    self->V = get_number(mesh, material, "V");

    return &(self->element);
}

static void gap_set_position(void* pself, mesh_t mesh)
{
    gap_t* self = (gap_t*) pself;
    double* x1 = mesh_node(mesh, self->node[0])->x;
    double* x2 = mesh_node(mesh, self->node[1])->x;

    double dx0 = x2[0] - x1[0];
    double dx1 = x2[1] - x1[1];
    double dx2 = x2[2] - x2[2];

    self->relpos[0] = dx0;
    self->relpos[1] = dx1;
    self->relpos[2] = dx2;

    self->g0 = sqrt(dx0*dx0 + dx1*dx1 + dx2*dx2);
}

static void gap_vars(void* pself, vars_mgr_t vars)
{
    gap_t* self = (gap_t*) pself;

    self->vars[0] = vars_node(vars, self->node[0], "x");
    self->vars[1] = vars_node(vars, self->node[0], "y");
    self->vars[2] = vars_node(vars, self->node[0], "rz");

    self->vars[3] = vars_node(vars, self->node[0], "x");
    self->vars[4] = vars_node(vars, self->node[0], "y");
    self->vars[5] = vars_node(vars, self->node[0], "rz");
}

static void gap_R(void* pself, assemble_matrix_t* R,
                  assemble_matrix_t* x,
                  assemble_matrix_t* v,
                  assemble_matrix_t* a)
{
/*
    gap_t* self = (gap_t*) pself;
    double u[6];
    double Rlocal[6];

    memset(u, 0, 6*sizeof(double));
    assemble_matrix_add(x, self->vars, 6, u);

    double Ke1 = self->e0 * self->h * self->V * self->V;
    double Ke2 = 0.65 * Ke1 / self->w;

    double rpx = self->relpos[0] + u[0] - u[3];
    double rpy = self->relpos[0] + u[1] - u[4];

    double a = sqrt(rpx*rpx + rpy*rpy);
    double t = u[2] - u[5];

    compute_forces(F, dF, Ke1, Ke2, a, t, self->l);

    Rlocal[0] =  F[0] * rpx / a;
    Rlocal[1] =  F[0] * rpy / a;
    Rlocal[2] =  F[1];

    Rlocal[3] = -Rlocal[0];
    Rlocal[4] = -Rlocal[1];
    Rlocal[5] = -F[1];

    assemble_matrix_add(R, self->vars, 6, Rlocal);
*/
}

static void gap_dR(void* pself, assemble_matrix_t* dR,
                   double cx, assemble_matrix_t* x,
                   double cv, assemble_matrix_t* v,
                   double ca, assemble_matrix_t* a)
{
/*
    gap_t* self = (gap_t*) pself;
    double u[6];
    double dRlocal[36];

    memset(u, 0, 6*sizeof(double));
    assemble_matrix_add(x, self->vars, 6, u);

    double Ke1 = self->e0 * self->h * self->V * self->V;
    double Ke2 = 0.65 * Ke1 / self->w;

    double rpx = self->relpos[0] + u[0] - u[3];
    double rpy = self->relpos[0] + u[1] - u[4];

    double a = sqrt(rpx*rpx + rpy*rpy);
    double t = u[2] - u[5];

    compute_forces(F, dF, Ke1, Ke2, a, t, self->l);

    dRlocal[0]  =               dR[0] * rpx * rpx / a / a;
    dRlocal[7]  =               dR[0] * rpy * rpy / a / a;
    dRlocal[1]  = dRlocal[6]  = dR[0] * rpx * rpy / a / a;

    dRlocal[21] =               dR[3] * rpx * rpx / a / a;
    dRlocal[28] =               dR[3] * rpy * rpy / a / a;
    dRlocal[22] = dRlocal[27] = dR[3] * rpx * rpy / a / a;





    Rlocal[0] =  F[0] * rpx / a;
    Rlocal[1] =  F[0] * rpy / a;
    Rlocal[2] =  F[1];

    Rlocal[3] = -Rlocal[0];
    Rlocal[4] = -Rlocal[1];
    Rlocal[5] = -F[1];


    assemble_matrix_add(dR, self->vars, 6, dRlocal);
*/
}

static void gap_output(void* pself, netout_t* netout)
{
    gap_t* self = (gap_t*) pself;

    netout_string        (netout, "model",  "gap");
    netout_int_matrix    (netout, "node",   self->node,   1, 2);
    netout_double        (netout, "l",      self->l);
    netout_double        (netout, "w",      self->w);
    netout_double        (netout, "h",      self->h);
    netout_double        (netout, "V",      self->V);
    netout_double_matrix (netout, "relpos", self->relpos, 1, 3);
    netout_int_matrix    (netout, "vars",   self->vars,   1, 6);
}

void model_gap_register(model_mgr_t model_mgr)
{
    model_element_t model;
    memset(&model, 0, sizeof(model));

    model.init         = gap_init;
    model.set_position = gap_set_position;
    model.vars         = gap_vars;
    model.R            = gap_R;
    model.dR           = gap_dR;
    model.output       = gap_output;

    model_mgr_add_element(model_mgr, "gap2d", &model);
}

