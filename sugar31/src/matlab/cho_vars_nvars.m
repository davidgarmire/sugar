function [nvars] = cho_vars_nvars(mesh)
nvars = sugarmex('vars_nvars', mesh);
