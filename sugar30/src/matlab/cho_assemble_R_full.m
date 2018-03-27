function [R] = cho_assemble_R_full(mesh, x, v, a)

if nargin == 1
    [R] = sugarmex('assemble_R', mesh, 1);
else
    [R] = sugarmex('assemble_R', mesh, 1, x);
end
