#include <sugar.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <model_gap.h>
#include <affine.h>
#include <mesh.h>
#include <vars.h>
#include <assemble.h>
#include <netdraw.h>
#include <netout.h>

typedef struct gap_t {
    element_t element;
    int node[2];
    int vars[6];
    double relpos[3];
    double e0;
    double l, w, h;
    double V;
} gap_t;


static element_t* gap_init(mesh_t mesh, const char* model,
                                   model_element_t* modelfunc)
{
    gap_t* self = (gap_t*) mempool_cget(mesh_pool(mesh), sizeof(*self));
    material_t* material;

    self->element.data = self;
    self->element.model = modelfunc;

    mesh_param_get_nodes(mesh, self->node, 2);
    material = mesh_param_get_material(mesh, "material");

    self->e0 = mesh_param_get_number(mesh, material, "permittivity");
    self->l  = mesh_param_get_number(mesh, material, "l");
    self->w  = mesh_param_get_number(mesh, material, "w");
    self->h  = mesh_param_get_number(mesh, material, "h");
    self->V  = mesh_param_get_number(mesh, material, "V");

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
}

static void gap_vars(void* pself, vars_mgr_t vars)
{
    gap_t* self = (gap_t*) pself;

    self->vars[0] = vars_node(vars, self->node[0], "x");
    self->vars[1] = vars_node(vars, self->node[0], "y");
    self->vars[2] = vars_node(vars, self->node[0], "rz");

    self->vars[3] = vars_node(vars, self->node[1], "x");
    self->vars[4] = vars_node(vars, self->node[1], "y");
    self->vars[5] = vars_node(vars, self->node[1], "rz");
}

static void gap_R(void* pself, assemble_matrix_t* R,
                          assemble_matrix_t* x,
                          assemble_matrix_t* v,
                          assemble_matrix_t* accel)
{
    int NPTS = 5;
    
    double gauss_pt[] = { 
        -0.9061798459,
        -0.5384693101,
         0           ,
         0.5384693101,
         0.9061798459 };
    
    double gauss_wt[] = {
         0.2369268851,
         0.4786286705,
         0.5688888889,
         0.4786286705,
         0.2369268851 };
    

    gap_t* self = (gap_t*) pself;
    double u[6];
    double Rlocal[6];
    double F, M;
    double rpx, rpy, a, t;
    int i;

    double Ke1 = self->e0 * self->h * self->V * self->V;
    double Ke2 = 0.65 * Ke1 / self->h;
    double L = self->l;

    memset(u, 0, 6*sizeof(double));
    assemble_matrix_add(x, self->vars, 6, u);

    rpx = self->relpos[0] + u[0] - u[3];
    rpy = self->relpos[1] + u[1] - u[4];

    a = sqrt(rpx*rpx + rpy*rpy);
    t = u[2] - u[5];

    rpx /= a;
    rpy /= a;

    /* Use Gauss quadrature to integrate the pressure load
     * to get force and moment magnitudes
     */
    F = 0;
    M = 0;
    for (i = 0; i < NPTS; ++i) {
        double xi = gauss_pt[i] * L/2;
        double gi = a + t*xi;
        double Fi = (Ke1/gi + Ke2)/gi;
        F += gauss_wt[i] * L/2 * (Fi     );
        M += gauss_wt[i] * L/2 * (Fi * xi);
    }

    Rlocal[0] =  F * rpx;
    Rlocal[1] =  F * rpy;
    Rlocal[2] =  M;

    Rlocal[3] = -Rlocal[0];
    Rlocal[4] = -Rlocal[1];
    Rlocal[5] = -M;

    assemble_matrix_add(R, self->vars, 6, Rlocal);
}

static void gap_dR(void* pself, assemble_matrix_t* dR,
                           double cx, assemble_matrix_t* x,
                           double cv, assemble_matrix_t* v,
                           double ca, assemble_matrix_t* accel)
{
    int NPTS = 5;
    
    double gauss_pt[] = { 
        -0.9061798459,
        -0.5384693101,
         0           ,
         0.5384693101,
         0.9061798459 };
    
    double gauss_wt[] = {
         0.2369268851,
         0.4786286705,
         0.5688888889,
         0.4786286705,
         0.2369268851 };
    

    gap_t* self = (gap_t*) pself;
    double u[6];
    double dRlocal[36];

    double dFda, dFdt;
    double dMda, dMdt;
    double rpx, rpy, a, t;
    int i;

    double Ke1 = self->e0 * self->h * self->V * self->V;
    double Ke2 = 0.65 * Ke1 / self->h;
    double L = self->l;

    memset(u, 0, 6*sizeof(double));
    assemble_matrix_add(x, self->vars, 6, u);

    rpx = self->relpos[0] + u[0] - u[3];
    rpy = self->relpos[1] + u[1] - u[4];

    a = sqrt(rpx*rpx + rpy*rpy);
    t = u[2] - u[5];

    /* Use Gauss quadrature to integrate the pressure load
     * to get force and moment magnitudes
     */
    dFda = 0;
    dMda = 0; dMdt = 0;
    for (i = 0; i < NPTS; ++i) {
        double xi = gauss_pt[i] * L/2;
        double gi = a + t*xi;
        double dFi = -(2*Ke1/gi + Ke2)/gi/gi;
        dFda += gauss_wt[i] * L/2 * (dFi          );
        dMda += gauss_wt[i] * L/2 * (dFi * xi     );
        dMdt += gauss_wt[i] * L/2 * (dFi * xi * xi);
    }
    dFdt = dMda;  /* By symmetry */

    rpx /= -a;  /* = da / d(delta x) */
    rpy /= -a;  /* = da / d(delta y) */

    #define K(i,j) dRlocal[(i-1)+6*(j-1)]

    K(1,1)          =  (dFda *  rpx) * rpx;
    K(2,1) = K(1,2) =  (dFda *  rpx) * rpy;
    K(3,1) = K(1,3) =  (dMda *  rpx);
    K(4,1) = K(1,4) = -(dFda *  rpx) * rpx;
    K(5,1) = K(1,5) = -(dFda *  rpx) * rpy;
    K(6,1) = K(1,6) = -(dMda *  rpx);

    K(2,2)          =  (dFda *  rpy) * rpy;
    K(3,2) = K(2,3) =  (dMda *  rpy);
    K(4,2) = K(2,4) = -(dFda *  rpy) * rpx;
    K(5,2) = K(2,5) = -(dFda *  rpy) * rpy;
    K(6,2) = K(2,6) = -(dMda *  rpy);

    K(3,3)          =  (dMdt *  1  );
    K(4,3) = K(3,4) = -(dFdt *  1  ) * rpx;
    K(5,3) = K(3,5) = -(dFdt *  1  ) * rpy;
    K(6,3) = K(3,6) = -(dMdt *  1  );

    K(4,4)          = -(dFda * -rpx) * rpx;
    K(5,4) = K(4,5) = -(dFda * -rpx) * rpy;
    K(6,4) = K(4,6) = -(dMda * -rpx);

    K(5,5)          = -(dFda * -rpy) * rpy;
    K(6,5) = K(5,6) = -(dMda * -rpy);

    K(6,6)          = -(dMdt * -1  );

    #undef K

    assemble_matrix_add(dR, self->vars, 6, dRlocal);
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
    netout_double        (netout, "e0",     self->e0);
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

