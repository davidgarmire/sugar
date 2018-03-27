function [id] = cho_vars_lookup_node(mesh, node_id, name)
id = sugarmex('vars_lookup_node', mesh, node_id, name);
