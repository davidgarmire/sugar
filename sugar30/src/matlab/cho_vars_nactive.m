function [nactive] = cho_vars_nactive(mesh)
nactive = sugarmex('vars_nactive', mesh);
