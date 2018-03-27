% Algorithm:
% Reference x is always the direction from beam 0 to beam 1
% If it looks at all possible, make the reference y direction in the x-y
% plane, and the reference z direction generally in the positive z.
% If that doesn't look feasible, make the reference y direction in the
% x-z plane and the reference z direction generally in the positive y.

%TODO: Add rotation around axis

function [param, R] = beam_postpos(param, R, pos)

if isfield(param, 'l')
  return
end


% -- First, get the beam end points in the local coordinates of the
%    nearest enclosing subnet

beam_dir = R'*(pos(:,2) - pos(:,1));
beam_l = norm(beam_dir);
param.l = beam_l;


% -- Reference x direction is along the beam

rx = beam_dir / beam_l;   


% -- Reference y and z directions are a little trickier

cutoff = 1e-8;
rx_proj = [rx(1); rx(2); 0];

if (norm(rx_proj) > cutoff)

  % If possible, the reference y should lie in the subnet x-y plane,
  % orthogonal to the projection of the reference x.

  ry = [-rx(2); rx(1); 0];
  ry = ry / norm(ry);

  rz = [0; 0; 1];
  rz = rz - (rz'*ry)*ry - (rz'*rx)*rx;
  rz = rz / norm(rz);

else

  % If the beam sticks nearly exactly out-of-plane, make the
  % reference y direction line in the subnet x-z plane

  ry = [rx(3); 0; -rx(1)];
  ry = ry / norm(ry);

  rz = [0; 1; 0];
  rz = rz - (rz'*ry)*ry - (rz'*rx)*rx;
  rz = rz / norm(rz);
  
end


% -- Now construct the rotation from local to subnet coordinates

Rnew = [rx ry rz];


% -- And finally construct the rotation from local to global

R = R*Rnew;
