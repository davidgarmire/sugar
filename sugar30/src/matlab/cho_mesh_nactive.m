function [n] = cho_mesh_nactive(mesh)
n = sugarmex('mesh_nactive', mesh);
