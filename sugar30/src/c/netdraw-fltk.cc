#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

extern "C" {
#include "netdraw.h"
#include "mesh.h"
#include "affine.h"
}

#include <Fl/fl_draw.h>
#include "netdraw-fltk.hh"

#define get_disp(i) \
    ((x == NULL || vindex == NULL || vindex[i] < 0) ? 0 : x[vindex[i]])

#define NPTS 10

#define min(a,b) (((a)<(b)) ? (a) : (b))
#define max(a,b) (((a)>(b)) ? (a) : (b))

static void    line (void* pself, double* x1, double* x2);
static double* get_x(void* pself);

static netdraw_fun_t fltk_draw_fun = {line, NULL, NULL, get_x};

static void line(void* pself, double* x1, double* x2)
{
    DrawBox* self = (DrawBox*) pself;
    double pts[6];

    memcpy(pts + 0, x1, 3*sizeof(double));
    memcpy(pts + 3, x2, 3*sizeof(double));

    affine_apply(self->toview,   pts + 0);
    affine_apply(self->toscreen, pts + 0);
    affine_apply(self->toview,   pts + 3);
    affine_apply(self->toscreen, pts + 3);

    fl_line((int) pts[0], (int) pts[1], 
            (int) pts[3], (int) pts[4]);

}

static double* get_x(void* pself)
{
    DrawBox* self = (DrawBox*) pself;
    return self->disp;
}

void DrawBox::set_transforms()
{
    int i, n;
    double xmin, xmax, ymin, ymax, scale;

    xmin = ymin = 1;
    xmax = ymax = -1;
    n = mesh_num_nodes(mesh);
    for (i = 1; i <= n; ++i) {
        mesh_node_t* node = mesh_node(mesh, i);
        xmin = min(xmin, node->x[0]);
        xmax = max(xmax, node->x[0]);
        ymin = min(ymin, node->x[1]);
        ymax = max(ymax, node->x[1]);
    }

    scale = 1/max(xmax-xmin, ymax-ymin);

    affine_identity(toview);
    affine_translate_xyz(toview, -(xmax+xmin)/2, -(ymax+ymin)/2, 0);
    affine_scale(toview, scale);
    affine_identity(toscreen);
    toscreen[4] = -1;

    double border = 50;
    double mindim = min(w(), h());
    affine_scale(toscreen, mindim - 2*border);
    affine_translate_xyz(toscreen, x()+w()/2, y()+h()/2, 0);
}

void DrawBox::set_mesh(mesh_t mesh)
{
    this->mesh = mesh;
    set_transforms();
    redraw();
}

void DrawBox::set_disp(double* disp)
{
    this->disp = disp;
    redraw();
}

void DrawBox::draw()
{
    netdraw_gc_t netdraw;
    netdraw.methods = &fltk_draw_fun;
    netdraw.data    = this;

    draw_box();

    if (mesh != NULL) {
        fl_color(FL_BLACK);
        fl_push_clip(x(), y(), w(), h());

        int n = mesh_num_elements(mesh);
        for (int i = 1; i <= n; ++i)
            element_display( mesh_element(mesh, i), &netdraw );

        fl_pop_clip();
    }
}

void DrawBox::resize(int x, int y, int w, int h)
{
    Fl_Box::resize(x, y, w, h);
    if (mesh)
        set_transforms();
}

int DrawBox::handle(int event)
{
    double xdelta, ydelta;

    switch (event) {
    case FL_PUSH:
        if (Fl::event_button() == FL_LEFT_MOUSE) {

            // start rotation
            pushed_wh = min(w(), h());
            pushed_x  = Fl::event_x();
            pushed_y  = Fl::event_y();

        } else if (Fl::event_button() == FL_RIGHT_MOUSE) {

            // reset to initial view
            set_transforms();
            redraw();

        } else if (Fl::event_button() == FL_MIDDLE_MOUSE) {

            // double zoom factor about clicked location
            double x_translate = x() + w()/2 - Fl::event_x();
            double y_translate = y() + h()/2 - Fl::event_y();

            x_translate += toscreen[9];
            y_translate += toscreen[10];

            affine_scale(toscreen, 2);

            toscreen[9]  = x_translate;
            toscreen[10] = y_translate;

            redraw();

        }
        return 1;

    case FL_DRAG:

        if (Fl::event_button() != FL_LEFT_MOUSE)
            return 1;

        // do rotation
        xdelta = 2*M_PI * (Fl::event_x() - pushed_x) / pushed_wh;
        if (xdelta > M_PI / 18 || xdelta < M_PI / 18) {
            pushed_x = Fl::event_x();
            affine_rotate_oy(toview, xdelta);
            redraw();
        }

        ydelta = 2*M_PI * (Fl::event_y() - pushed_y) / pushed_wh;
        if (ydelta > M_PI / 18 || ydelta < M_PI / 18) {
            pushed_y = Fl::event_y();
            affine_rotate_ox(toview, ydelta);
            redraw();
        }

        return 1;

    case FL_RELEASE:
        // end rotation
        return 1;

    default:
        return Fl_Box::handle(event);
    }
}

