#line 11 "mesh-mex.nw"
#ifndef MESH_MEX_H
#define MESH_MEX_H

#include <mex.h>
#include "mesh.h"

void mesh_mex_register();
void mesh_mex_shutdown();
int  mesh_mex_handle_tag();

mesh_t       mx_to_mesh    (const mxArray* array);
mesh_node_t* mx_to_node    (mesh_t mesh, const mxArray* array);
material_t*  mx_to_material(mesh_t mesh, const mxArray* array);
mxArray*     mx_from_param (mesh_param_t* param);
mxArray*     mx_from_params(mesh_t mesh);

#endif /* MESH_MEX_H */
