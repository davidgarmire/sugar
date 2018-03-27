#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "model-beam2d.h"
#include "affine.h"
#include "mesh.h"
#include "vars.h"
#include "assemble.h"
#include "netdraw.h"
#include "netout.h"

typedef struct beam2d_t {
    element_t element;
    int node[2];
    int vars[6];
    double density;
    double Kaxial;
    double EI, EA;
    double l, w, h;
    double transform[12];
} beam2d_t;

static double get_number(mesh_t mesh, material_t* material,
                         const char* name)
{
    mesh_param_t* param = mesh_param_byname(mesh, name);
    if (param == NULL && material != NULL && material->model != NULL &&
            material->model->param != NULL)
        param = (material->model->param)(material->data, name);
    return mesh_param_number(mesh, param);    
}

static void compute_transformation(double *transform,
                                   double* x1, double* x2, double l)
{
    double c = (x2[0] - x1[0])/l;
    double s = (x2[1] - x1[1])/l;
    memset(transform, 0, 12*sizeof(double));

    #define T(i,j) (transform[((j)-1)*3+((i)-1)])

    T(1,1) =  c;
    T(1,2) = -s;
    T(2,1) =  s;
    T(2,2) =  c;
    T(3,3) =  1;

    T(1,4) = x1[0];
    T(2,4) = x1[1];
    T(3,4) = x1[2];

    #undef T
}

static void beam2d_transform(double* transform, double* Klocal)
{
    int i;

    for (i = 0; i < 6; ++i)
        affine_apply_A_stride(transform, Klocal + i,      6);
    for (i = 0; i < 6; ++i)
        affine_apply_A_stride(transform, Klocal + i + 18, 6);
    for (i = 0; i < 6; ++i)
        affine_apply_A_stride(transform, Klocal + i*6,     1);
    for (i = 0; i < 6; ++i)
        affine_apply_A_stride(transform, Klocal + i*6 + 3, 1);
}

static element_t* beam2d_init(mesh_t mesh, const char* model,
                              model_element_t* modelfunc)
{
    beam2d_t* self = (beam2d_t*) 
        mempool_cget(mesh_pool(mesh), sizeof(beam2d_t));

    double rho, A, E;
    material_t* material;

    self->element.data = self;
    self->element.model = modelfunc;

    if (mesh_num_param_nodes(mesh) != 2)
        mesh_error(mesh, "Incorrect number of nodes for beam2d");
    self->node[0] = mesh_param_node(mesh, 0);
    self->node[1] = mesh_param_node(mesh, 1);
    material = mesh_param_material(mesh, mesh_param_byname(mesh, "material"));
    rho = get_number(mesh, material, "density");
    E = get_number(mesh, material, "Youngsmodulus");
    self->w = get_number(mesh, material, "w");
    self->h = get_number(mesh, material, "h");

    A = self->w * self->h;
    self->EA      = E*A;
    self->EI      = E*A* self->w * self->w /12;
    self->density = rho*A;

    return &(self->element);
}

static void beam2d_set_position(void* pself, mesh_t mesh)
{
    beam2d_t* self = (beam2d_t*) pself;
    double* x1 = mesh_node(mesh, self->node[0])->x;
    double* x2 = mesh_node(mesh, self->node[1])->x;

    self->l  = (x2[0]-x1[0])*(x2[0]-x1[0]);
    self->l += (x2[1]-x1[1])*(x2[1]-x1[1]);
    self->l  = sqrt(self->l);
    self->Kaxial  = self->EA / self->l;
    compute_transformation(self->transform, x1, x2, self->l);
}

static void beam2d_vars(void* pself, vars_mgr_t vars)
{
    beam2d_t* self = (beam2d_t*) pself;

    self->vars[0] = vars_node(vars, self->node[0], "x");
    self->vars[1] = vars_node(vars, self->node[0], "y");
    self->vars[2] = vars_node(vars, self->node[0], "rz");

    self->vars[3] = vars_node(vars, self->node[1], "x");
    self->vars[4] = vars_node(vars, self->node[1], "y");
    self->vars[5] = vars_node(vars, self->node[1], "rz");
}

static void beam2d_R(void* pself, assemble_matrix_t *R, 
                     assemble_matrix_t* x,
                     assemble_matrix_t* v,
                     assemble_matrix_t* a)
{
    beam2d_t* self = (beam2d_t*) pself;
    double Kaxial = self->Kaxial;
    double EI     = self->EI;
    double l      = self->l, l2 = l*l, l3 = l2*l;
    double crhoA  = self->w * self->h * self->density;
    double c      = EI/(l*l*l);

    double Rlocal[6];

    if (x) {
        double xlocal[6];
        memset(xlocal, 0, 6 * sizeof(double));

        assemble_matrix_add(x, self->vars, 6, xlocal);
        affine_apply_AT(self->transform, xlocal + 0);
        affine_apply_AT(self->transform, xlocal + 3);

        Rlocal[0] = Kaxial * (xlocal[0] - xlocal[3]);
        
        Rlocal[1] = c * (12 * ( xlocal[1] - xlocal[4] ) + 
                         6*l* ( xlocal[2] + xlocal[5] ));

        Rlocal[2] = c * (6*l* ( xlocal[1] - xlocal[4] ) +
                         l*l*(4*xlocal[2] + 2*xlocal[5] ));


        Rlocal[3] = Kaxial * (xlocal[3] - xlocal[0]);

        Rlocal[4] = c * (12 *  (-xlocal[1] + xlocal[4] ) + 
                         -6*l* ( xlocal[2] + xlocal[5] ));

        Rlocal[5] = c * (6*l* ( xlocal[1] - xlocal[4] ) +
                         l*l*(2*xlocal[2] + 4*xlocal[5] ));
    }

    if (a) {
        double xlocal[6];
        memset(xlocal, 0, 6 * sizeof(double));

        assemble_matrix_add(x, self->vars, 6, xlocal);
        affine_apply_AT(self->transform, xlocal + 0);
        affine_apply_AT(self->transform, xlocal + 3);

        assert(0);
        /* TODO: Fill in the inertial force computation */
        Rlocal[0] = Kaxial * (xlocal[0] - xlocal[3]);
        
        Rlocal[1] = c * (12 * ( xlocal[1] - xlocal[4] ) + 
                         6*l* ( xlocal[2] + xlocal[5] ));

        Rlocal[2] = c * (6*l* ( xlocal[1] - xlocal[4] ) +
                         l*l*(4*xlocal[2] + 2*xlocal[5] ));


        Rlocal[3] = Kaxial * (xlocal[3] - xlocal[0]);

        Rlocal[4] = c * (12 *  (-xlocal[1] + xlocal[4] ) + 
                         -6*l* ( xlocal[2] + xlocal[5] ));

        Rlocal[5] = c * (6*l* ( xlocal[1] - xlocal[4] ) +
                         l*l*(2*xlocal[2] + 4*xlocal[5] ));
    }

    affine_apply_A(self->transform, Rlocal + 0);
    affine_apply_A(self->transform, Rlocal + 3);
    assemble_matrix_add(R, self->vars, 6, Rlocal);
}

static void beam2d_dR(void* pself, assemble_matrix_t* dR, 
                      double cx, assemble_matrix_t* x,
                      double cv, assemble_matrix_t* v,
                      double ca, assemble_matrix_t* a)
{
    beam2d_t* self = (beam2d_t*) pself;
    double Kaxial = self->Kaxial;
    double EI     = self->EI;
    double l      = self->l, l2 = l*l, l3 = l2*l;
    double crhoA  = self->w * self->h * self->density;
    double c      = EI/(l*l*l);

    if (cx) {
        double Klocal[36];
        memset(Klocal, 0, 36*sizeof(double));
        c *= cx;

        #define K(i,j) (Klocal[(j)*6+(i)-7])
        K(1,1) = K(4,4) =  cx * Kaxial;
        K(4,1) = K(1,4) = -cx * Kaxial;

        /* Fill in the 11 block, first */
        K(2,2) =           12*c;
        K(3,3) =            4*c*l2;
        K(2,3) = K(3,2) =   6*c*l;

        /* Now fill in the 22 block */
        K(5,5) =           12*c;
        K(6,6) =            4*c*l2;
        K(5,6) = K(6,5) =  -6*c*l;

        /* Now the off-diagonal blocks */
        K(5,2) = K(2,5) = -12*c;
        K(6,3) = K(3,6) =   2*c*l2;
        K(6,2) = K(2,6) =   6*c*l;
        K(5,3) = K(3,5) =  -6*c*l;

        #undef K

        beam2d_transform(self->transform, Klocal);
        assemble_matrix_add(dR, self->vars, 6, Klocal);
    }

    if (ca) {
        double Mlocal[36];
        memset(Mlocal, 0, 36*sizeof(double));
        crhoA *= ca;

        #define M(i,j) (Mlocal[(j)*6+(i)-7])
        M(1,1) = M(4,4) =  crhoA * l / 3;
        M(4,1) = M(1,4) =  crhoA * l / 6;

        /* Fill in the 11 block, first */
        M(2,2) =           crhoA * 12/l3;
        M(3,3) =           crhoA * 4 /l;
        M(2,3) = M(3,2) =  crhoA * 6 /l2;

        /* Now fill in the 22 block */
        M(5,5) =           crhoA * 12/l3;
        M(6,6) =           crhoA * 4 /l;
        M(5,6) = M(6,5) = -crhoA * 6 /l2;

        /* Now the off-diagonal blocks */
        M(5,2) = M(2,5) = -crhoA * 12/l3;
        M(6,3) = M(3,6) =  crhoA * 2 /l;
        M(6,2) = M(2,6) =  crhoA * 6 /l2;
        M(5,3) = M(3,5) = -crhoA * 6 /l2;

        #undef M

        beam2d_transform(self->transform, Mlocal);
        assemble_matrix_add(dR, self->vars, 6, Mlocal);
    }

}

static void beam2d_output(void* pself, netout_t* netout)
{
    beam2d_t* self = (beam2d_t*) pself;

    netout_string        (netout, "model",     "beam2d");
    netout_int_matrix    (netout, "node",      self->node, 1, 2);
    netout_double        (netout, "density",   self->density);
    netout_double        (netout, "Kaxial",    self->Kaxial);
    netout_double        (netout, "EI",        self->EI);
    netout_double        (netout, "l",         self->l);
    netout_double        (netout, "w",         self->w);
    netout_double        (netout, "h",         self->h);
    netout_double_matrix (netout, "transform", self->transform, 3, 4);
    netout_int_matrix    (netout, "vars",      self->vars, 1, 6);
}

static void beam2d_display(void* pself, netdraw_gc_t* gc)
{
    beam2d_t* self = (beam2d_t*) pself;
    int vars[12];
    int i;

    for (i = 0; i < 12; ++i)
        vars[i] = -1;

    vars[0]  = self->vars[0];
    vars[1]  = self->vars[1];
    vars[5]  = self->vars[2];
    vars[6]  = self->vars[3];
    vars[7]  = self->vars[4];
    vars[11] = self->vars[5];

    netdraw_beam(gc, self->transform, self->l, self->w, self->h, vars);
}

void model_beam2d_register(model_mgr_t model_mgr)
{
    model_element_t model;
    memset(&model, 0, sizeof(model));

    model.init         = beam2d_init;
    model.set_position = beam2d_set_position;
    model.vars         = beam2d_vars;
    model.R            = beam2d_R;
    model.dR           = beam2d_dR;
    model.output       = beam2d_output;
    model.display      = beam2d_display;

    model_mgr_add_element(model_mgr, "beam2d", &model);
}

