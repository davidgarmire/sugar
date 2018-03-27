function [vars] = cho_vars_lookup(mesh)
vars = sugarmex('vars_lookup', mesh);
