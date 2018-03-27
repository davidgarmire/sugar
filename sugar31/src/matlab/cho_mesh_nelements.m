% Return the number of elements in a netlist.
%
% Inputs:
%   net -- pointer to the netlist structure
%
% Outputs:
%   count -- count of the number of elements in the netlist

function [count] = cho_mesh_nelements(handle)
count = sugarmex('mesh_nelements', handle);
