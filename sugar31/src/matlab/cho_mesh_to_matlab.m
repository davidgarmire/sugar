% Create a Matlab representation of a netlist data structure maintained
% internally by SUGAR.
%
% Inputs:
%   net   -- a pointer to the netlist data structure
%
% Outputs:
%   meshm -- a Matlab representation of the netlist data structure

function [meshm] = cho_mesh_to_matlab(handle)
meshm = sugarmex('mesh_to_matlab', handle);
