function [node] = cho_mesh_node_lookup(handle, nodename)
node = sugarmex('mesh_node_lookup', handle, nodename);
