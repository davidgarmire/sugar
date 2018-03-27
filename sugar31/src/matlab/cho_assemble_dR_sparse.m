function [dR] = cho_assemble_dR_sparse(mesh, c, x, v, a)

if nargin == 1
    [dR] = sugarmex('assemble_dR_sparse', mesh, [1 0 0]);
elseif nargin == 2
    [dR] = sugarmex('assemble_dR_sparse', mesh, c);
elseif nargin == 3
    [dR] = sugarmex('assemble_dR_sparse', mesh, c, x);
elseif nargin == 4
    [dR] = sugarmex('assemble_dR_sparse', mesh, c, x, v);
elseif nargin == 5
    [dR] = sugarmex('assemble_dR_sparse', mesh, c, x, v, a);
end
