% 2D electrostatic gap actuator model
%
% Treats fringing fields by multiplying by a correction factor.

% 2D gap mechanical model with contact forces; voltage as input;
% R: transform local coordinates(x,y,z) to global;

% By Ningning Zhou on 12/10/2000;

function [output] = MF_gap2d(flag, R, params, q, t, nodes, varargin);
% Modified from template by Ningning Zhou on 10/4/2000;

switch(flag)

case 'vars'

  output.dynamic = {1 {'x' 'y' 'rz' } ;
                    2 {'x' 'y' 'rz' } ;
                    3 {'x' 'y' 'rz' } ;
                    4 {'x' 'y' 'rz' } };


case 'check'

  if (~isfield(params, 'density')       | ...
      ~isfield(params, 'fluid')         | ...
      ~isfield(params, 'viscosity')     | ...
      ~isfield(params, 'Youngsmodulus') | ...
      ~isfield(params, 'permittivity'))
    output = 'Missing parameters typically specified in process file';
  elseif ~isfield(params, 'l')
    output = 'Missing length';
  elseif ~isfield(params, 'w1')
    output = 'Missing width w1';
  elseif ~isfield(params, 'w2')
    output = 'Missing width w2';
  elseif ~isfield(params, 'gap')
    output = 'Missing gap';
  elseif ~isfield(params, 'h') 
    output = 'Missing height';
  elseif ~isfield(params, 'V')
    output = 'Missing V';
  else
    output = [];
  end


case 'M'
  output = beam_matrices('M', R, params); 

case 'D'
  output = beam_matrices('D', R, params);

case 'K'
  output = beam_matrices('K', R, params);

case 'F'
  output = compute_forces(flag, R(1:2,1:2), params, q, t);
 
case 'dFdx'
  output = compute_forces(flag, R(1:2,1:2), params, q, t);
  
case 'pos'
  l = params.l;
  g = params.gap + (params.w1 + params.w2)/2;

  output = R * ...
      [0  l   0  l;
       0  0  -g -g;
       0  0   0  0];


case 'display'
  GapDisplay(R, params, q, nodes);

otherwise

  output = [];

end


% ---

function [M] = beam_matrices(flag, R, params)

params.w = params.w1;
MB1 = MF_beam2d(flag, R, params);

params.w = params.w2;
MB2 = MF_beam2d(flag, R, params);

Z = zeros(6);

M = [MB1    Z;
       Z  MB2];


% ---

function [output] = compute_forces(flag, R, params, q, t)
% By Ningning Zhou on 10/4/2000;

V = params.V;   % In DC analysis, only one DC voltage is required.
gap = params.gap;           % Gap width
l = params.l;               % Length of the beams
e0 = params.permittivity;   % Permittivity of free space
h = params.h;

x = q(1:12);
x1 = R'*x(1:2);    % Local displacements of node 1
x2 = R'*x(4:5);    % Local displacements of node 2
x3 = R'*x(7:8);    % Local displacements of node 3
x4 = R'*x(10:11);  % Local displacements of node 4
q1 = [x1;x(3);x2;x(6)]; % Local dispacements of beam1;
q2 = [x3;x(9);x4;x(12)]; % Local dispacements of beam2;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Limiting algorithm for contact forces; 
% set the minimum gap distance;
if isfield(params, 'limit') 
   limit = params.limit;
else
   limit = 0.055e-6;
end

d13 = q1(2) - q2(2) + gap;     % distance between node 1 and 3; 
d24 = q1(5) - q2(5) + gap;     % distance between node 2 and 4;
% set a limit for gap distance; limiting algorithm for Newton-Raphson;
uFlag = 0;
if (d13 < limit)
  deltaD13 = 0.5*((limit + 0.002e-6) - d13);
  q1(2) = q1(2) + deltaD13;
  q2(2) = q2(2) - deltaD13;
  uFlag = 1;
end
if (d24 < limit)
  deltaD24 = 0.5*((limit + 0.002e-6) - d24);
  q1(5) = q1(5) + deltaD24;
  q2(5) = q2(5) - deltaD24;
  uFlag = 1;
end
%if (uFlag == 1)
%  u1Lim = t' * q1;         % from locl to global; 
%  u2Lim = t' * q2; 
%  % Output.u is the modified unknown vector(global);           
%  Output.u = [u1Lim(1:3);u(4);u1Lim(4:6);u(8);u2Lim(1:3);u(12);u2Lim(4:6);u(16)];
%end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

q_loc = [q1;q2];

% Electrostatic forces(local) on the gap node 1 and 2; 
% Fringe forces are zeros. 
% Use linear approximation for the shape of gap beams;
[fe] = gapElec(e0, l, h, gap, V, q_loc);

% Contact forces;
[fc] = gapCont(gap, h, q_loc);

F_beam1 = fe + fc; % Total external forces on node 1 and 2;

switch (flag)
case 'F'
  % Transfer forces to global;
  % Force on node 1, y direction, Moment on node 1
  F1 = [R*F_beam1(1:2); F_beam1(3)];  
  % Force on node 2, y direction, Moment on node 2;
  F2 = [R*F_beam1(4:5); F_beam1(6)];  
  % Forces and moments on nodes 3 and 4 are opposite those on 1 and 2
  output = [F1; F2; -F1; -F2];
  
case 'dFdx'
  delta_h = 1e-9;
  for i=1:12
    del_q  = q_loc(i) - delta_h;
    q = [q_loc(1:(i-1)); del_q; q_loc((i+1):12)];
    [Fe_delta] = gapElec(e0, l, h, gap, V, q);
    [Fc_delta] = gapCont(gap, h, q);
    for j=1:6
      J_beam1(j,i) = (F_beam1(j) - Fe_delta(j) - Fc_delta(j))/delta_h;
    end
  end

  R6 = eye(6);
  R6(1:2,1:2) = R;
  R6(4:5,4:5) = R;

  J11 = [R6*J_beam1(1:6,1:6)*R6'];
  J12 = [R6*J_beam1(1:6,7:12)*R6'];

  % Form the complete Jacobia
  output = [ J11  J12;
            -J11 -J12]; 
end


% ---

function GapDisplay(R, params, q, nodes)

% Set up width and displacement (with zero padding for z, rx, ry)
% and display the first beam
q1([1 2 6 7 8 12]) = q(1:6);
displaybeam(q1, nodes(1).pos, R, params.l, params.w1, params.h);

% Set up width and displacement (with zero padding for z, rx, ry)
% and display the second beam
q2([1 2 6 7 8 12]) = q(7:12);
displaybeam(q2, nodes(3).pos,R, params.l, params.w2, params.h);

% ---


