function [R] = cho_assemble_R(mesh, x, v, a)

if nargin == 1
    [R] = sugarmex('assemble_R', mesh, 0);
else
    [R] = sugarmex('assemble_R', mesh, 0, x);
end
