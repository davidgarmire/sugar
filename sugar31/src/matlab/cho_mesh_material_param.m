% Return a parameter retrieved from a mesh material.
%
% Inputs:
%   net         -- pointer to netlist structure
%   material_id -- material number
%   name        -- parameter name
%
% Outputs:
%   param       -- material parameter value

function [param] = cho_mesh_material_param(handle, material, name)
param = sugarmex('mesh_material_param', handle, material, name);
