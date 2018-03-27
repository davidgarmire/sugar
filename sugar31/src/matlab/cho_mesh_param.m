function [param] = cho_mesh_param(mesh, arg1, arg2)

if nargin == 1
    param = sugarmex('mesh_params', mesh);
elseif nargin == 2
    param = sugarmex('mesh_param', mesh, arg1);
elseif nargin == 3
    param = sugarmex('mesh_param', mesh, arg2);
    if (isempty(param))
        param = sugarmex('mesh_material_param', mesh, arg1, arg2)
    end
end
