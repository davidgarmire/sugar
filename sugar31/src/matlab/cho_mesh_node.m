% Return a description (name and position) of a netlist node.
%
% Inputs:
%   net     -- netlist handle
%   node_id -- node number
%
% Outputs:
%   node.name -- name of the node
%       .x    -- position of the node

function [node] = cho_mesh_node(handle, node_id)

node.name = sugarmex('mesh_node_name', handle, node_id);
node.x    = sugarmex('mesh_node_pos',  handle, node_id);
