% 2D small-deflection linear beam model
%
% Input parameters:
%   l, w  - beam length and width
%   h - beam height (often specified in process parameters)
%   density, fluid, viscosity, Youngmodulus -
%       material parameters specified as part of the process info
%
% Nodes/variables:
%   The two end nodes are conceptually in the middle of the two
%   w-by-h end faces, which are l units apart.  Each node has
%   the usual spatial displacement variables (x, y, rz)

function [output] = MF_beam2d(flag, R, params, q, t, nodes, varargin);

switch(flag)

case 'vars'

  output.dynamic = {1 {'x' 'y' 'rz'};
                    2 {'x' 'y' 'rz'}};

  output.ground = {1 {'z' 'rx' 'ry'};
                   2 {'z' 'rx' 'ry'}};

case 'check'

  if (~isfield(params, 'density')       | ...
      ~isfield(params, 'fluid')         | ...
      ~isfield(params, 'viscosity')     | ...
      ~isfield(params, 'Youngsmodulus'))
    output = 'Missing parameters typically specified in process file';
%  elseif ~isfield(params, 'l')
%    output = 'Missing length';
  elseif ~isfield(params, 'w')
    output = 'Missing width';
  elseif ~isfield(params, 'h') 
    output = 'Missing height';
  else
    output = [];
  end


case 'M'

  rho = params.density;
  l = params.l;
  A = params.w * params.h;

  % Make rotation matrix 2D
  R = R(1:2,1:2);
  R(3,3) = 1;

  M11 = R * [140,    0,     0;
               0,  156,  22*l;
               0, 22*l, 4*l*l] * R';

  M12 = R * [70,      0,       0;
              0,     54,   -13*l;
              0,   13*l,  -3*l*l] * R';

  M22 = R * [140,     0,     0;
               0,   156, -22*l;
               0, -22*l, 4*l*l] * R';

  a1 = rho*A*l/420;
  output = a1 * [M11  M12;
                 M12' M22];


case 'D'

  l = params.l;
  w = params.w;
  v = params.viscosity;
  f = params.fluid; 

  a1 = v*l*w/f/420;

  % Make rotation matrix 2D
  R = R(1:2,1:2);
  R(3,3) = 1;

  D11 = R * [140,     0,     0;
               0,   156,  22*l;
               0,  22*l, 4*l*l] * R';

  D12 = R * [70,     0,      0;
              0,    54,  -13*l;
              0,  13*l, -3*l*l] * R';

  D22 = R * [140,     0,       0;
               0,   156,   -22*l;
               0, -22*l,   4*l*l] * R';

  output = a1 * [D11  D12;
                 D12' D22];


case 'K'

  E = params.Youngsmodulus;
  l = params.l;
  w = params.w;
  h = params.h;

  A = w*h;
  I = w^3*h/12;
  c = E*I/l^3;

  % Make rotation matrix 2D
  R = R(1:2,1:2);
  R(3,3) = 1;

  K11 = R * [E*A/l,      0,       0;
                 0,   12*c,   6*c*l;
                 0,  6*c*l, 4*c*l*l] * R';

  K12 = R * [-E*A/l,      0,       0;
                  0,  -12*c,   6*c*l;
                  0, -6*c*l, 2*c*l*l] * R';

  K22 = R * [E*A/l,      0,       0;
                 0,   12*c,  -6*c*l;
                 0, -6*c*l, 4*c*l*l] * R';

  output = [K11  K12;
            K12' K22];

case 'pos'

  % Compute relative positions of beam nodes

  if (isfield(params, 'l'))
    output = R * [0 params.l;
                  0 0;
                  0 0];
  else
    output = [];
  end

case 'postpos'

  [params, R] = beam_postpos(params, R, [nodes(1).pos, nodes(2).pos]);
  output.params = params;
  output.R = R;

case 'display'

  % Set up width and displacement (with zero padding for z, rx, ry)
  % and display the beam
  q1([1 2 6 7 8 12]) = q(1:6);
  displaybeam(q1, nodes(1).pos, R, params.l, params.w, params.h);

otherwise

  output = [];

end
