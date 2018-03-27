function [count] = cho_mesh_nmaterials(handle)
count = sugarmex('mesh_nmaterials', handle);
