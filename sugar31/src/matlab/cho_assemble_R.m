function [R] = cho_assemble_R(mesh, x, v, a)

if nargin == 1
    [R] = sugarmex('assemble_R', mesh, 0);
elseif nargin == 2
    [R] = sugarmex('assemble_R', mesh, 0, x);
elseif nargin == 3
    [R] = sugarmex('assemble_R', mesh, 0, x, v);
elseif nargin == 4
    [R] = sugarmex('assemble_R', mesh, 0, x, v, a);
else
    error('Incorrect number of arguments to cho_assemble_R');
end
