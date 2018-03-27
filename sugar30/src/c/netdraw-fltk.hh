#ifndef NETDRAW_FLTK_H
#define NETDRAW_FLTK_H

#include <Fl/Fl.H>
#include <Fl/Fl_Box.H>

extern "C" { 
#include "mesh.h"
}

class DrawBox : public Fl_Box {
public:
    DrawBox(int x, int y, int w, int h) :
        Fl_Box(x, y, w, h),
        mesh(NULL),
        disp(NULL)
    {
        box(FL_DOWN_BOX);
    }

    void set_mesh(mesh_t mesh);
    void set_disp(double* disp);

    void draw();
    int handle(int event);
    void resize(int x, int y, int w, int h);

    mesh_t mesh;
    double* disp;
    double toview[12];
    double toscreen[12];
    int pushed_x;
    int pushed_y;
    double pushed_wh;

    void set_transforms();
};


#endif /* NETDRAW_FLTK_H */

