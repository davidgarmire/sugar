function [id] = cho_vars_lookup_branch(mesh, elt_id, var_number)
id = sugarmex('vars_lookup_branch', mesh, elt_id, var_number);
