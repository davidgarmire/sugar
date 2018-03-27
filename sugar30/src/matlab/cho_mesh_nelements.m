function [count] = cho_mesh_nnodes(handle)
count = sugarmex('mesh_nelements', handle);
