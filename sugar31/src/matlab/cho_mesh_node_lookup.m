% Find the index of a netlist node with a particular name.
%
% Inputs:
%   net      -- pointer to a netlist structure
%   nodename -- name of a node in the netlist
%
% Outputs:
%   node_id  -- node number.  If no match is found, node_id = [].

function [node] = cho_mesh_node_lookup(handle, nodename)
node = sugarmex('mesh_node_lookup', handle, nodename);
