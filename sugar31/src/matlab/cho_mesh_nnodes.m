% Return the number of materials in a netlist.
%
% Inputs:
%   net -- pointer to the netlist structure
%
% Outputs:
%   count -- count of the number of materials in the netlist

function [count] = cho_mesh_nnodes(handle)
count = sugarmex('mesh_nnodes', handle)
