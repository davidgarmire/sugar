function [node] = cho_mesh_node(handle, node_id)

node.name = sugarmex('mesh_node_name', handle, node_id);
node.x    = sugarmex('mesh_node_pos',  handle, node_id);
