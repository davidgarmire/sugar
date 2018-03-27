#ifndef NETDRAW_H
#define NETDRAW_H

typedef struct netdraw_fun_t {
    void (*line)(void* self, double* x1, double* x2);
    void (*plane)(void* self, double* xpts, int npts);
    void (*beam)(void* self, double* transform, 
                         double l, double w, double h, 
                         int* vindex);
    double* (*get_x)(void* self);
} netdraw_fun_t;

typedef struct netdraw_gc_t {
    netdraw_fun_t* methods;
    void* data;
} netdraw_gc_t;

void netdraw_line(netdraw_gc_t* gc, double* x1, double* x2);
void netdraw_line_xyz(netdraw_gc_t* gc, 
                              double x1, double y1, double z1,
                              double x2, double y2, double z2);
void netdraw_plane(netdraw_gc_t* gc, double* xpts, int npts);
void netdraw_plane_quad(netdraw_gc_t* gc, 
                                double* x1, double* x2,
                                double* x3, double* x4);
void netdraw_beam(netdraw_gc_t* gc, double* transform,
                          double l, double w, double h, int* vindex);
double* netdraw_get_x(netdraw_gc_t* gc);

#endif /* NETDRAW_H */
