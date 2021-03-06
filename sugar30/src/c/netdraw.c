#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "affine.h"
#include "netdraw.h"

#define get_disp(i) \
    ((x == NULL || vindex == NULL || vindex[i] < 0) ? 0 : x[vindex[i]])

#define NPTS 10

static void beam_get_local(double* transform, double* xlocal, 
                           double* x, int* vindex)
{
    int i;

    for (i = 0; i < 12; ++i)
        xlocal[i] = get_disp(i);

    for (i = 0; i < 12; i += 3)
        affine_apply_AT(transform, &(xlocal[i]));
}

static void hermite(double* f, int stride, double L, int n,
                    double f0, double f00, double fL, double fLL)
{
    double f0L   = ( fL   - f0   ) / L;
    double f00L  = ( f0L  - f00  ) / L;
    double f0LL  = ( fLL  - f0L  ) / L;
    double f00LL = ( f0LL - f00L ) / L;
    int i;

    for (i = 0; i < n; ++i) {
        double s = i*L / (n-1);
        *f = f0 + s*(f00 + s*(f00L + (s-L)*f00LL));
        f += stride;
    }
}

static void beam_points(double* pts, double l, double w, double h, 
                        double* x, double* transform)
{
    double x1  = x[0], y1  = x[1],  z1  = x[2];
    double x2  = x[6], y2  = x[7],  z2  = x[8];
    double rx1 = x[3], ry1 = x[4],  rz1 = x[5];
    double rx2 = x[9], ry2 = x[10], rz2 = x[11];

    double rotx1[12];
    double rotx2[12];
    double A[12], *Ap = A;
    double B[12], *Bp = B;

    int i, j;

    A[0] = 0; A[1]  =  w/2;  A[2]  = -h/2;
    A[3] = 0; A[4]  =  w/2;  A[5]  =  h/2;
    A[6] = 0; A[7]  = -w/2;  A[8]  =  h/2;
    A[9] = 0; A[10] = -w/2;  A[11] = -h/2;
    memcpy(B, A, 12*sizeof(double));

    affine_identity (rotx1);
    affine_identity (rotx2);
    affine_rotate_ox(rotx1, ry1);
    affine_rotate_ox(rotx2, ry2);

    for (i = 0; i < 4; ++i) {
        affine_apply(rotx1, Ap);
        affine_apply(rotx2, Bp);
        for (j = 0; j < NPTS; ++j) 
            pts[j*12] = ((NPTS-1-j)*(Ap[0]+x1) + (j)*(Bp[0]+x2+l)) / (NPTS-1);
        hermite(pts+1, 12, l, NPTS,  Ap[1]+y1,  rz1, Bp[1]+y2,    rz2);
        hermite(pts+2, 12, l, NPTS,  Ap[2]+z1, -ry1, Bp[2]+z2,   -ry2);
        Ap  += 3;
        Bp  += 3;
        pts += 3;
    }

    pts -= 12;
    for (i = 0; i < 12*NPTS; i += 3) {
        affine_apply(transform, pts + i);
    }
}

static void beam_draw(netdraw_gc_t* gc, double* pts)
{
    int i, j;

    /* Draw end caps */
    netdraw_plane(gc, pts, 4);
    netdraw_plane(gc, pts + 12*(NPTS-1), 4);

    /* Draw exterior of each segment */
    for (i = 1; i < NPTS; ++i) {
        for (j = 0; j < 12; j += 3) {
            int ix1 = (j+0) % 12;
            int ix2 = (j+3) % 12;
            netdraw_plane_quad(gc, 
                               &pts[ix1], &pts[ix1 + 12],
                               &pts[ix2 + 12], &pts[ix2]);
        }
        pts += 12;
    }
}

void netdraw_line(netdraw_gc_t* gc, double* x1, double* x2)
{
    assert(gc->methods->line);
    gc->methods->line(gc->data, x1, x2);
}

void netdraw_line_xyz(netdraw_gc_t* gc,
                      double x1, double y1, double z1,
                      double x2, double y2, double z2)
{
    double p1[3], p2[3];
    p1[0] = x1, p1[1] = y1, p1[2] = z1;
    p2[0] = x2, p2[1] = y2, p2[2] = z2;
    netdraw_line(gc, p1, p2);
}

void netdraw_plane(netdraw_gc_t* gc, double* xpts, int npts)
{
    if (gc->methods->plane) {
        gc->methods->plane(gc->data, xpts, npts);
    } else {
        int i;
        netdraw_line(gc, xpts, xpts + 3*(npts-1));
        for (i = 0; i < npts-1; ++i) {
            netdraw_line(gc, xpts, xpts + 3);
            xpts += 3;
        }
    }
}

void netdraw_plane_quad(netdraw_gc_t* gc, 
                        double* x1, double* x2,
                        double* x3, double* x4)
{
    double pts[12];
    memcpy(pts + 0, x1, 3*sizeof(double));
    memcpy(pts + 3, x2, 3*sizeof(double));
    memcpy(pts + 6, x3, 3*sizeof(double));
    memcpy(pts + 9, x4, 3*sizeof(double));
    netdraw_plane(gc, pts, 4);
}

void netdraw_beam(netdraw_gc_t* gc, double* transform,
                  double l, double w, double h, int* vindex)
{
    if (gc->methods->beam) {
        gc->methods->beam(gc->data, transform, l, w, h, vindex);
    } else {
        double* x = netdraw_get_x(gc);
        double xlocal[12];
        double pts[NPTS*12];

        beam_get_local(transform, xlocal, x, vindex);
        beam_points(pts, l, w, h, xlocal, transform);
        beam_draw(gc, pts);
    }
}

double* netdraw_get_x(netdraw_gc_t* gc)
{
    if (gc->methods->get_x) {
        return gc->methods->get_x(gc->data);
    } else {
        return NULL;
    }
}

