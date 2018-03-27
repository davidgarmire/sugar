function [nodes] = cho_mesh_param_nodes(mesh)
nodes = sugarmex('mesh_param_nodes', mesh);
