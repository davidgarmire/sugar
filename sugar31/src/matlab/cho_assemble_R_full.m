function [R] = cho_assemble_R_full(mesh, x, v, a)

if nargin == 1
    [R] = sugarmex('assemble_R', mesh, 1);
elseif nargin == 2
    [R] = sugarmex('assemble_R', mesh, 1, x);
elseif nargin == 3
    [R] = sugarmex('assemble_R', mesh, 1, x, v);
elseif nargin == 4
    [R] = sugarmex('assemble_R', mesh, 1, x, v, a);
else
    error('Incorrect number of arguments to cho_assemble_R');
end
