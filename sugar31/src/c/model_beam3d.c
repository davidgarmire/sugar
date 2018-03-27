#include <sugar.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <model_beam3d.h>
#include <affine.h>
#include <mesh.h>
#include <vars.h>
#include <assemble.h>
#include <netdraw.h>
#include <netout.h>

typedef struct beam3d_t {
    element_t element;
    int node[2];
    int vars[12];
    double E;
    double G;
    double poisson;
    double density;
    double fluid;
    double viscosity;
    double l, w, h;
    double up[3];
    double transform[12];
} beam3d_t;

static void compute_transformation(mesh_t mesh, double *transform,
                                   double* x1, double* x2, double l,
                                   double* up)
{
    #define T(i,j) (transform[((j)-1)*3+((i)-1)])

    double axis_dot_up;
    memset(transform, 0, 12*sizeof(double));


    /* Local x axis is the axis of the beam
     */
    T(1,1) = (x2[0] - x1[0]) / l;
    T(2,1) = (x2[1] - x1[1]) / l;
    T(3,1) = (x2[2] - x1[2]) / l;


    /* Local z axis is the normalized projection of 'up' onto the
     * plane perpendicular to the local x axis.
     */
    axis_dot_up = (T(1,1) * up[0] +
                   T(2,1) * up[1] +
                   T(3,1) * up[2]);

    T(1,3) = up[0] - axis_dot_up * T(1,1);
    T(2,3) = up[1] - axis_dot_up * T(2,1);
    T(3,3) = up[2] - axis_dot_up * T(3,1);

    l = sqrt(T(1,3) * T(1,3) +
             T(2,3) * T(2,3) +
             T(3,3) * T(3,3));

    if (l < 1e-8) {
        mesh_error(mesh, "Beam direction nearly coincides with 'up'");
    }

    T(1,3) /= l;
    T(2,3) /= l;
    T(3,3) /= l;


    /* Local y axis is cross(local z, local x)
     */
    T(1,2) = T(2,3)*T(3,1) - T(2,1)*T(3,3);
    T(2,2) = T(1,1)*T(3,3) - T(1,3)*T(3,1);
    T(3,2) = T(1,3)*T(2,1) - T(1,1)*T(2,3);


    /* Translation takes the local origin to the global position x1
     */
    T(1,4) = x1[0];
    T(2,4) = x1[1];
    T(3,4) = x1[2];

    #undef T
}

static void compute_mass(double* Mdata, int N, double mcoeff,
                         double L, double W, double H)
{
    double A  = H * W;
    double Iy = W * H*H*H / 12;
    double Iz =        H * W*W*W / 12;
    double Jx =        Iy + Iz;

    double a = mcoeff * ( 1.0 / 3        );
    double b = mcoeff * ( 13.0 / 35      );
    double c = mcoeff * ( 13.0 / 35      );
    double d = mcoeff * ( Jx / (3 * A)   );
    double e = mcoeff * ( L*L / 105      );
    double f = mcoeff * ( L*L / 105      );
    double g = mcoeff * ( 1.0 / 3        );
    double h = mcoeff * ( 13.0 / 35      );
    double i = mcoeff * ( 13.0 / 35      );
    double j = mcoeff * ( Jx / (3 * A)   );
    double k = mcoeff * ( L*L / 105      );
    double l = mcoeff * ( L*L / 105      );
    double m = mcoeff * (-11.0 * L / 210 );
    double n = mcoeff * ( 11.0 * L / 210 );
    double oo= mcoeff * ( 9.0 / 70       );
    double p = mcoeff * ( 13 * L / 420   );
    double q = mcoeff * ( 9.0 / 70       );
    double r = mcoeff * (-13.0 * L / 420 );
    double s = mcoeff * ( Jx / (6 * A)   );
    double t = mcoeff * (-13.0 * L / 420 );
    double u = mcoeff * (-L*L / 140      );
    double v = mcoeff * ( 1.0 / 6        );
    double w = mcoeff * (-L*L / 140      );

    #define M(ki,kj) Mdata[(kj-1)*N+(ki-1)]

    /* Form (1,1) block */
    M(1,1) += a;
    M(2,2) += b; M(2,6) += n; M(6,2) += n;
    M(3,3) += c; M(3,5) += m; M(5,3) += m;
    M(4,4) += d;
    M(5,5) += e;
    M(6,6) += f;

    /* Form (2,2) block */
    M(7,7)   += g;
    M(8,8)   += h; M(8,12) += -n; M(12,8) += -n;
    M(9,9)   += i; M(9,11) += -m; M(11,9) += -m;
    M(10,10) += j;
    M(11,11) += k;
    M(12,12) += l;

    /* Form (2,1) and (1,2) blocks */
    M(1,7)  += v;  M(7,1)  += v;
    M(2,8)  += oo; M(8,2)  += oo;  M(2,12) +=  t; M(12,2) +=  t;
    M(3,9)  += q;  M(9,3)  += q;   M(3,11) += -r; M(11,3) += -r;
    M(4,10) += s;  M(10,4) += s;
    M(5,11) += w;  M(11,5) += w;   M(5,9)  +=  r; M(9,5)  +=  r;
    M(6,12) += u;  M(12,6) += u;   M(6,8)  +=  p; M(8,6)  +=  p;

    #undef M
}

static void compute_stiffness(double* Kdata, int n, int cx, 
                              double E, double G, 
                              double L, double W, double H)
{
    double A  = W * H;
    double Iy = W * H*H*H / 12;
    double Iz = H * W*W*W / 12;

    double J = (W > H) ?
        W * H*H*H * (16.0/3 - 3.36*H/W * (1-(H/W)*(H/W)*(H/W)*(H/W)/12)) / 16 :
        H * W*W*W * (16.0/3 - 3.36*W/H * (1-(W/H)*(W/H)*(W/H)*(W/H)/12)) / 16;

    double a = cx * E * A / L;
    double b = cx * 12 * E * Iz / (L*L*L);
    double c = cx * 12 * E * Iy / (L*L*L);
    double d = cx * G * J / L;
    double e = cx * 4 * E * Iy / L;
    double f = cx * 2 * E * Iy / L;
    double g = cx * 4 * E * Iz / L;
    double h = cx * 2 * E * Iz / L;
    double i = cx * 6 * E * Iy / (L*L);
    double j = cx * 6 * E * Iz / (L*L);

    #define K(ki,kj) Kdata[(kj-1)*n+(ki-1)]

    /* (1,1) block */
    K(1,1) += a;
    K(2,2) += b; K(2,6) +=  j; K(6,2) +=  j;
    K(3,3) += c; K(3,5) += -i; K(5,3) += -i;
    K(4,4) += d;
    K(5,5) += e;
    K(6,6) += g;
   
    /* (2,2) block */
    K(7,7)   += a;
    K(8,8)   += b; K(8,12) += -j; K(12,8) += -j;
    K(9,9)   += c; K(9,11) +=  i; K(11,9) +=  i;
    K(10,10) += d;
    K(11,11) += e;
    K(12,12) += g;

    /* (1,2) and (2,1) blocks */
    K(1,7)  += -a; K(7,1)  += -a;
    K(2,8)  += -b; K(8,2)  += -b; K(2,12) +=  j; K(12,2) +=  j;
    K(3,9)  += -c; K(9,3)  += -c; K(3,11) += -i; K(11,3) += -i;
    K(4,10) += -d; K(10,4) += -d;
    K(5,11) +=  f; K(11,5) +=  f; K(5,9)  +=  i; K(9,5)  +=  i;
    K(6,12) +=  h; K(12,6) +=  h; K(6,8)  += -j; K(8,6)  += -j;

    #undef K
}

static void beam3d_transform(double* transform, double* Klocal)
{
    int i, j;

    for (i = 0; i < 12; ++i) {
        for (j = 0; j < 12; j += 3) {
            affine_apply_A_stride(transform, Klocal + j*12 + i, 12);
        }
    }
    
    for (i = 0; i < 12; i += 3) {
        for (j = 0; j < 12; ++j) {
            affine_apply_A_stride(transform, Klocal + j*12 + i, 1);
        }
    }
    
}

static element_t* beam3d_init(mesh_t mesh, const char* model,
                                      model_element_t* modelfunc)
{
    beam3d_t* self = (beam3d_t*) 
        mempool_cget(mesh_pool(mesh), sizeof(beam3d_t));

    material_t* material;
    mesh_param_t* param;

    self->element.data = self;
    self->element.model = modelfunc;

    mesh_param_get_nodes(mesh, self->node, 2);
    material = mesh_param_get_material(mesh, "material");

    self->E         = mesh_param_get_number(mesh, material, "Youngsmodulus");
    self->poisson   = mesh_param_get_number(mesh, material, "Poisson");
    self->density   = mesh_param_get_number(mesh, material, "density");
    self->viscosity = mesh_param_get_number(mesh, material, "viscosity");
    self->fluid     = mesh_param_get_number(mesh, material, "fluid");
    self->w         = mesh_param_get_number(mesh, material, "w");
    self->h         = mesh_param_get_number(mesh, material, "h");

    param = mesh_param_byname(mesh, material, "shearmodulus");
    self->G = (param ? 
               mesh_param_number(mesh, param) :
               (self->E) / (2 * (1+self->poisson)));
    
    param = mesh_param_byname(mesh, material, "up");
    
    if (param) {
        if (param->tag == MESH_PARAM_MATRIX && 
                param->val.m.m * param->val.m.n == 3) {
            memcpy(self->up, param->val.m.data, 3 * sizeof(double));
        } else {
            mesh_error(mesh, "up vector had wrong type or size");
        }
    } else {
        self->up[0] = 0;
        self->up[1] = 0;
        self->up[2] = 1;
    }
    

    return &(self->element);
}

static void beam3d_set_position(void* pself, mesh_t mesh)
{
    beam3d_t* self = (beam3d_t*) pself;
    double* x1 = mesh_node(mesh, self->node[0])->x;
    double* x2 = mesh_node(mesh, self->node[1])->x;

    self->l  = (x2[0]-x1[0])*(x2[0]-x1[0]);
    self->l += (x2[1]-x1[1])*(x2[1]-x1[1]);
    self->l += (x2[2]-x1[2])*(x2[2]-x1[2]);
    self->l  = sqrt(self->l);

    compute_transformation(mesh, self->transform, x1, x2, self->l, self->up);
}

static void beam3d_vars(void* pself, vars_mgr_t vars)
{
    beam3d_t* self = (beam3d_t*) pself;

    self->vars[ 0] = vars_node(vars, self->node[0], "x");
    self->vars[ 1] = vars_node(vars, self->node[0], "y");
    self->vars[ 2] = vars_node(vars, self->node[0], "z");
    self->vars[ 3] = vars_node(vars, self->node[0], "rx");
    self->vars[ 4] = vars_node(vars, self->node[0], "ry");
    self->vars[ 5] = vars_node(vars, self->node[0], "rz");

    self->vars[ 6] = vars_node(vars, self->node[1], "x");
    self->vars[ 7] = vars_node(vars, self->node[1], "y");
    self->vars[ 8] = vars_node(vars, self->node[1], "z");
    self->vars[ 9] = vars_node(vars, self->node[1], "rx");
    self->vars[10] = vars_node(vars, self->node[1], "ry");
    self->vars[11] = vars_node(vars, self->node[1], "rz");
}

static void beam3d_R(void* pself, assemble_matrix_t *R, 
                             assemble_matrix_t* x,
                             assemble_matrix_t* v,
                             assemble_matrix_t* a)
{
    beam3d_t* self = (beam3d_t*) pself;
    double dRlocal[144];
    double Rlocal[12];
    int i, j;

    memset(Rlocal, 0, 12 * sizeof(double));

    if (x) {
        double xlocal[12];
        memset(xlocal, 0, 12 * sizeof(double));
        assemble_matrix_add(x, self->vars, 12, xlocal);

        memset(dRlocal, 0, 144*sizeof(double));
        compute_stiffness(dRlocal, 12, 1, self->E, self->G, 
                          self->l, self->w, self->h);
        beam3d_transform(self->transform, dRlocal); 

        for (i = 0; i < 12; ++i) {
            for (j = 0; j < 12; ++j) {
                Rlocal[i] += dRlocal[j*12 + i] * xlocal[j];
            }
        }
    }

    if (v) {
        double xlocal[12];
        memset(xlocal, 0, 12 * sizeof(double));
        assemble_matrix_add(v, self->vars, 12, xlocal);

        memset(dRlocal, 0, 144*sizeof(double));
        compute_mass(dRlocal, 12, 
                     self->viscosity * self->l * self->w / self->fluid, 
                     self->l, self->w, self->h);
        beam3d_transform(self->transform, dRlocal); 

        for (i = 0; i < 12; ++i) {
            for (j = 0; j < 12; ++j) {
                Rlocal[i] += dRlocal[j*12 + i] * xlocal[j];
            }
        }
    }

    if (a) {
        double xlocal[12];
        memset(xlocal, 0, 12 * sizeof(double));
        assemble_matrix_add(a, self->vars, 12, xlocal);

        memset(dRlocal, 0, 144*sizeof(double));
        compute_mass(dRlocal, 12, 
                     self->density   * self->l * self->w * self->h,
                     self->l, self->w, self->h);
        beam3d_transform(self->transform, dRlocal); 

        for (i = 0; i < 12; ++i) {
            for (j = 0; j < 12; ++j) {
                Rlocal[i] += dRlocal[j*12 + i] * xlocal[j];
            }
        }
    }

    /* TODO: add thermal expansion, strain gradient terms 
     * Also might make the multiplies above more efficient.
     */

    assemble_matrix_add(R, self->vars, 12, Rlocal);
}

static void beam3d_dR(void* pself, assemble_matrix_t* dR, 
                              double cx, assemble_matrix_t* x,
                              double cv, assemble_matrix_t* v,
                              double ca, assemble_matrix_t* a)
{
    beam3d_t* self = (beam3d_t*) pself;
    double dRlocal[144];

    /* Coefficient for mass + proportional damping from squeeze-film */
    double mcoeff = (ca * self->density   * self->l * self->w * self->h +
                     cv * self->viscosity * self->l * self->w / self->fluid);

    memset(dRlocal, 0, 144*sizeof(double));

    if (cx) {
        compute_stiffness(dRlocal, 12, cx, self->E, self->G, 
                          self->l, self->w, self->h);
    }

    if (mcoeff) {
        compute_mass(dRlocal, 12, mcoeff, self->l, self->w, self->h);
    }

    beam3d_transform(self->transform, dRlocal); 
    assemble_matrix_add(dR, self->vars, 12, dRlocal);
}

static void beam3d_output(void* pself, netout_t* netout)
{
    beam3d_t* self = (beam3d_t*) pself;

    netout_string        (netout, "model",     "beam3d");
    netout_int_matrix    (netout, "node",      self->node, 1, 2);
    netout_double        (netout, "E",         self->E);
    netout_double        (netout, "poisson",   self->poisson);
    netout_double        (netout, "G",         self->G);
    netout_double        (netout, "density",   self->density);
    netout_double        (netout, "fluid",     self->fluid);
    netout_double        (netout, "viscosity", self->viscosity);
    netout_double        (netout, "l",         self->l);
    netout_double        (netout, "w",         self->w);
    netout_double        (netout, "h",         self->h);
    netout_double_matrix (netout, "up",        self->up, 3, 1);
    netout_double_matrix (netout, "transform", self->transform, 3, 4);
    netout_int_matrix    (netout, "vars",      self->vars, 1, 12);
}

static void beam3d_display(void* pself, netdraw_gc_t* gc)
{
    beam3d_t* self = (beam3d_t*) pself;
    int vars[12];
    int i;

    for (i = 0; i < 12; ++i)
        vars[i] = self->vars[i];

    netdraw_beam(gc, self->transform, self->l, self->w, self->h, vars);
}

void model_beam3d_register(model_mgr_t model_mgr)
{
    model_element_t model;
    memset(&model, 0, sizeof(model));

    model.init         = beam3d_init;
    model.set_position = beam3d_set_position;
    model.vars         = beam3d_vars;
    model.R            = beam3d_R;
    model.dR           = beam3d_dR;
    model.output       = beam3d_output;
    model.display      = beam3d_display;

    model_mgr_add_element(model_mgr, "beam3d", &model);
}

