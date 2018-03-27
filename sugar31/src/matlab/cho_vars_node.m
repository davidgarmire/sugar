function [var_id] = cho_vars_node(varmgr, node, name)
var_id = sugarmex('vars_node', varmgr, node, name);
