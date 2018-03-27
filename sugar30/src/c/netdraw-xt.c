#ifdef HAS_X11

#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include <sys/types.h>
#include <unistd.h>

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Core.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>

#include "netdraw-xt.h"
#include "netdraw.h"
#include "mesh.h"
#include "affine.h"

typedef struct xtnet_gc_t {
    mesh_t mesh;
    double* x;

    double toview[12];
    double toscreen[12];

    Display* disp;
    Window   win;
    GC       gc;
} xtnet_gc_t;

#define min(a,b) (((a)<(b)) ? (a) : (b))
#define max(a,b) (((a)>(b)) ? (a) : (b))

#define FRAME_IN_SPACE 100
#define FRAMEW 510
#define FRAMEH 510

#define FRAME_INW (FRAMEW - 2*FRAME_IN_SPACE)
#define FRAME_INH (FRAMEH - 2*FRAME_IN_SPACE)

static void line(void* pself, double* x1, double* x2);
static double* get_x(void* pself);

static netdraw_fun_t xt_draw_fun = {line, NULL, NULL, get_x};

static int    argc;
static char** argv;

static void draw_all(xtnet_gc_t* self)
{
    int i, n;
    netdraw_gc_t netdraw;
    netdraw.methods = &xt_draw_fun;
    netdraw.data    = self;

    n = mesh_num_elements(self->mesh);
    for (i = 1; i <= n; ++i)
        element_display( mesh_element(self->mesh, i), &netdraw );
}

static void set_transforms(mesh_t mesh, double* toview, double* toscreen)
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

    affine_translate_xyz(toscreen, 0.5, 0.5, 0);
    affine_scale_x(toscreen, FRAME_INW);
    affine_scale_y(toscreen, FRAME_INH);
    affine_translate_xyz(toscreen, FRAME_IN_SPACE, FRAME_IN_SPACE, 0);

}

static void quit_func(Widget w, XtPointer client, XtPointer call)
{
    exit(0);
}

static void redisplay_event(Widget w, XtPointer client, XExposeEvent* ev,
                            Boolean* continue_dispatch)
{
    xtnet_gc_t* self = (xtnet_gc_t*) client;

    if (ev->count != 0)
        return;

    self->disp = XtDisplay(w);
    self->win  = XtWindow(w);
    XClearWindow(self->disp, self->win);

    self->gc = XCreateGC(self->disp, self->win, 0, NULL);
    XSetForeground(self->disp, self->gc, 1);
    XSetBackground(self->disp, self->gc, 0);
    draw_all(self);
    XFreeGC(self->disp, self->gc);
}

static void keypress_event(Widget w, XtPointer client, XKeyEvent* ev,
                           Boolean* continue_dispatch)
{
    xtnet_gc_t* self = (xtnet_gc_t*) client;
    double inc = (M_PI / 18);

    switch (ev->keycode) {
        case 98:    /* Up    */
            affine_rotate_ox(self->toview, -inc);
            break;

        case 104:   /* Down  */
            affine_rotate_ox(self->toview, inc);
            break;
            
        case 100:   /* Left  */
            affine_rotate_oy(self->toview, inc);
            break;
            
        case 102:   /* Right */
            affine_rotate_oy(self->toview, -inc);
            break;

        case 99:    /* PgUp  */
            affine_rotate_oz(self->toview, -inc);
            break;

        case 105:   /* PgDn  */
            affine_rotate_oz(self->toview, inc);
            break;
    }

    self->disp = XtDisplay(w);
    self->win  = XtWindow(w);
    XClearWindow(self->disp, self->win);

    self->gc = XCreateGC(self->disp, self->win, 0, NULL);
    XSetForeground(self->disp, self->gc, 1);
    XSetBackground(self->disp, self->gc, 0);
    draw_all(self);
    XFreeGC(self->disp, self->gc);
}
static void line(void* pself, double* x1, double* x2)
{
    xtnet_gc_t* self = (xtnet_gc_t*) pself;
    double pts[6];

    memcpy(pts + 0, x1, 3*sizeof(double));
    memcpy(pts + 3, x2, 3*sizeof(double));

    affine_apply(self->toview,   pts + 0);
    affine_apply(self->toscreen, pts + 0);    
    affine_apply(self->toview,   pts + 3);
    affine_apply(self->toscreen, pts + 3);

    XDrawLine(self->disp, self->win, self->gc, 
              (int) pts[0], (int) pts[1], (int) pts[3], (int) pts[4]);
}

static double* get_x(void* pself)
{
    xtnet_gc_t* self = (xtnet_gc_t*) pself;
    return self->x;
}

void xtnetdraw_init(int my_argc, char** my_argv)
{
    argc = my_argc;
    argv = my_argv;
}

void xtnetdraw(mesh_t mesh, double* x)
{
    Widget toplevel;
    Widget box;
    Widget drawing;
    Widget quit;
    xtnet_gc_t* self;

    int n;
    Arg wargs[10];

    if (fork() != 0)
        return;

    self = (xtnet_gc_t*) malloc(sizeof(struct xtnet_gc_t));

    self->mesh = mesh;
    self->x    = x;

    set_transforms(mesh, self->toview, self->toscreen);
    toplevel = XtInitialize(argv[0], "drawing", NULL, 0, &argc, argv);

    box     = XtCreateManagedWidget("box",  boxWidgetClass,  toplevel, NULL, 0);
    drawing = XtCreateManagedWidget("draw", coreWidgetClass,    box, NULL, 0);
    quit    = XtCreateManagedWidget("quit", commandWidgetClass, box, NULL, 0);
    n = 0;
    XtSetArg(wargs[n], XtNheight, FRAMEH); n++;
    XtSetArg(wargs[n], XtNwidth,  FRAMEW); n++;
    XtSetValues(drawing, wargs, n);

    XtAddCallback(quit, XtNcallback, quit_func, NULL);
    XtAddEventHandler(drawing, ExposureMask, FALSE,
                      (XtEventHandler) redisplay_event, self);
    XtAddEventHandler(drawing, KeyPressMask, FALSE,
                      (XtEventHandler) keypress_event, self);

    XtRealizeWidget(toplevel);
    XtMainLoop();
}


#endif /* HAS_X11 */
