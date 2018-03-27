function [meshm] = cho_mesh_to_matlab(handle)
meshm = sugarmex('mesh_to_matlab', handle);
