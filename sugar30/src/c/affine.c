#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "affine.h"

void affine_identity(double* T)
{
    memset(T, 0, 12*sizeof(double));
    T[0] = 1;
    T[4] = 1;
    T[8] = 1;
}

void affine_translate(double* T, double* c)
{
    T[9 ] += c[0];
    T[10] += c[1];
    T[11] += c[2];
}

void affine_translate_xyz(double* T, double x, double y, double z)
{
    T[9 ] += x;
    T[10] += y;
    T[11] += z;
}

void affine_rotate_ox(double* T, double ox)
{
    double c = cos(ox);
    double s = sin(ox);
    int i;

    for (i = 0; i < 4; ++i) {
        double y =  c*T[1] + s*T[2];
        double z = -s*T[1] + c*T[2];
        T[1] = y;
        T[2] = z;
        T += 3;
    }
}

void affine_rotate_oy(double* T, double oy)
{
    double c =  cos(oy);
    double s = -sin(oy);
    int i;

    for (i = 0; i < 4; ++i) {
        double x =  c*T[0] + s*T[2];
        double z = -s*T[0] + c*T[2];
        T[0] = x;
        T[2] = z;
        T += 3;
    }
}

void affine_rotate_oz(double* T, double oz)
{
    double c = cos(oz);
    double s = sin(oz);
    int i;

    for (i = 0; i < 4; ++i) {
        double x =  c*T[0] + s*T[1];
        double y = -s*T[0] + c*T[1];
        T[0] = x;
        T[1] = y;
        T += 3;
    }
}

void affine_scale(double* T, double s)
{
    int i;
    for (i = 0; i < 12; ++i)
        *T++ *= s;
}

void affine_scale_x(double* T, double s)
{
    int row = 0;
    int i;
    for (i = 0; i < 4; ++i, T += 3)
        T[row] *= s;
}

void affine_scale_y(double* T, double s)
{
    int row = 1;
    int i;
    for (i = 0; i < 4; ++i, T += 3)
        T[row] *= s;
}

void affine_scale_z(double* T, double s)
{
    int row = 2;
    int i;
    for (i = 0; i < 4; ++i, T += 3)
        T[row] *= s;
}

void affine_compose(double* T, double* S)
{
    int i;
    for (i = 0; i < 3; ++i) {
        double y[3];

        y[0] = T[0]*S[0] + T[3]*S[1] + T[6]*S[2];
        y[1] = T[1]*S[0] + T[4]*S[1] + T[7]*S[2];
        y[2] = T[2]*S[0] + T[5]*S[1] + T[8]*S[2];

        S[0] = y[0];
        S[1] = y[1];
        S[2] = y[2];

        S += 3;
    }

    affine_apply(T, S);
}

void affine_apply(double* T, double* x)
{
    affine_apply_stride(T, x, 1);
}

void affine_apply_stride(double* T, double* x, int stride)
{
    double x0 = x[0*stride];
    double x1 = x[1*stride];
    double x2 = x[2*stride];

    x[0*stride] = T[0]*x0 + T[3]*x1 + T[6]*x2 + T[9 ];
    x[1*stride] = T[1]*x0 + T[4]*x1 + T[7]*x2 + T[10];
    x[2*stride] = T[2]*x0 + T[5]*x1 + T[8]*x2 + T[11];
}

void affine_apply_A(double* T, double* x)
{
    affine_apply_A_stride(T, x, 1);
}

void affine_apply_A_stride(double* T, double* x, int stride)
{
    double x0 = x[0*stride];
    double x1 = x[1*stride];
    double x2 = x[2*stride];

    x[0*stride] = T[0]*x0 + T[3]*x1 + T[6]*x2;
    x[1*stride] = T[1]*x0 + T[4]*x1 + T[7]*x2;
    x[2*stride] = T[2]*x0 + T[5]*x1 + T[8]*x2;
}

void affine_apply_AT(double* T, double* x)
{
    affine_apply_AT_stride(T, x, 1);
}

void affine_apply_AT_stride(double* T, double* x, int stride)
{
    double x0 = x[0*stride];
    double x1 = x[1*stride];
    double x2 = x[2*stride];

    x[0] = T[0]*x0 + T[1]*x1 + T[2]*x2;
    x[1] = T[3]*x0 + T[4]*x1 + T[5]*x2;
    x[2] = T[6]*x0 + T[7]*x1 + T[8]*x2;
}

