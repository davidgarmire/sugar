function [param] = cho_mesh_material_param(handle, material, name)
param = sugarmex('mesh_material_param', handle, material, name);
