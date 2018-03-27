#ifdef HAS_XDR

#include <stdio.h>
#include <rpc/xdr.h>

#include "netdraw-java.h"
#include "netdraw.h"

static void write_int(XDR* xdrs, int i)
{
    xdr_int(xdrs, &i);
}

static void write_double(XDR* xdrs, double d)
{
    float ftmp = (float) d;
    xdr_float(xdrs, &ftmp);
}

static void beam(void* xdrsp, double* transform, double l, double w, double h,
                 int* vindex)
{
    XDR*  xdrs = (XDR*) xdrsp;
    int   i;

    write_int(xdrs, 1);

    for (i = 0; i < 9; ++i)
        write_double(xdrs, transform[i]);

    write_double(xdrs, l);
    write_double(xdrs, w);
    write_double(xdrs, h);

    for (i = 10; i < 12; ++i)
        write_double(xdrs, transform[i]);

    for (i = 0; i < 12; ++i)
        write_int(xdrs, vindex[i]);
}

void javawrite(FILE* fp, mesh_t mesh, double* x, int num_vars)
{
    XDR xdrs;
    int i, n;
    netdraw_fun_t java_draw_fun = { NULL, NULL, beam, NULL };
    netdraw_gc_t  java_gc;

    java_gc.methods = &java_draw_fun;
    java_gc.data    = &xdrs;

    xdrstdio_create(&xdrs, fp, XDR_ENCODE);
    n = 0;
    for (i = 1; i <= mesh_num_elements(mesh); ++i) {
        element_t* element = mesh_element(mesh, i);
        if (element->model->display)
            ++n;
    }

    write_int(&xdrs, n);

    for (i = 1; i <= mesh_num_elements(mesh); ++i)
        element_display( mesh_element(mesh, i), &java_gc );

    for (i = 0; i < num_vars; ++i)
        write_double(&xdrs, (x != NULL) ? x[i] : 0);
    xdr_destroy(&xdrs);    
}


#endif /* HAS_XDR */
