% 2D gap electrical mechanical model with contact force;
% R: transform local coodinates(x,y,z) to global;

% Created from gap template by Ningning Zhou on 11/5/00;
function [output] = MF_gap2dforce(flag, R, params, q, t, nodes, varargin)

switch(flag)

case 'vars'

  output.dynamic = {1 {'x' 'y' 'rz' 'e'} ;
                    2 {'x' 'y' 'rz' 'e'} ;
                    3 {'x' 'y' 'rz' 'e'} ;
                    4 {'x' 'y' 'rz' 'e'} };


case 'check'

  if (~isfield(params, 'permittivity'))
    output = 'Missing parameters typically specified in process file';
  elseif ~isfield(params, 'l')
    output = 'Missing length';
  elseif ~isfield(params, 'w1')
    output = 'Missing width w1';
  elseif ~isfield(params, 'w2')
    output = 'Missing width w2';
  elseif ~isfield(params, 'gap')
    output = 'Missing gap';
  else
    output = [];
 end
 
case 'F'
   % q = [x;x'] 
   output = compute_forces(flag, R(1:2,1:2), params, q, t);

case 'dFdx'
  % q = [x;x'] 
   output = compute_forces(flag, R(1:2,1:2), params, q, t);
  
case 'dFdxdot'
   % q= [x;x'] 
   output = compute_forces(flag, R(1:2,1:2), params, q, t);
  
case 'pos'

  l = params.l;
  g = params.gap + (params.w1 + params.w2)/2;

  output = R * ...
      [0  l   0  l;
       0  0  -g -g;
       0  0   0  0];

otherwise

  output = [];

end

% ----

function [output] = compute_forces(flag, R, params, q, t)
% By Ningning Zhou on 11/05/2000;

x = q(1:16);
% x(1)--x(3): displacements at node 1;
% x(4): electrical potential at node 1;
% x(5)--x(7): displacements at node 2;
% x(8): electrical potential at node 2;
% x(9)--x(11): displacements at node 3;
% x(12): electrical potential at node 3;
% x(13)--x(15): displacements at node 4;
% x(16): electrical potential at node 4;
   
u = [x(1:3); x(5:7); x(9:11); x(13:15)]; %global displacements;
params.V = abs(x(4) - x(12));    % e1-e3
[D1,D2] = compute_dFdxdot(R,params,x); %global;

switch(flag)
   
case 'F'
  [F] = MF_gap2d(flag, R, params, u, t); %global electrotatic forces; 
  output = [F(1:3);0; F(4:6);0; F(7:9);0; F(10:12);0];
  if length(q)>16
    xdot = q(17:32);
    output(4)= - D1*xdot;
    output(8)= - D2*xdot;
    output(12) = -output(4);
    output(16) = -output(8);
  end
  
case 'dFdx'
  [F] = MF_gap2d(flag, R, params, u, t); %global electrotatic forces;
  Z = zeros(3,1);
  output=[F(1:3,1:3)   Z F(1:3,4:6)   Z F(1:3,7:9)   Z F(1:3,10:12)   Z;
       Z'           0 Z'           0 Z'           0 Z'             0;
       F(4:6,1:3)   Z F(4:6,4:6)   Z F(4:6,7:9)   Z F(4:6,10:12)   Z;
       Z'           0 Z'           0 Z'           0 Z'             0;
       F(7:9,1:3)   Z F(7:9,4:6)   Z F(7:9,7:9)   Z F(7:9,10:12)   Z;
       Z'           0 Z'           0 Z'           0 Z'             0;
       F(10:12,1:3) Z F(10:12,4:6) Z F(10:12,7:9) Z F(10:12,10:12) Z;
       Z'           0 Z'           0 Z'           0 Z'             0];
      
  if (params.V ~=0)  
%    R = rot2local(params.oz);
    u1 = R'*u(1:2);    % Local displacements of node 1
    u2 = R'*u(4:5);    % Local displacements of node 2
    u3 = R'*u(7:8);    % Local displacements of node 3
    u4 = R'*u(10:11);  % Local displacements of node 4
    q1 = [u1;u(3);u2;u(6)]; % Local dispacements of beam1;
    q2 = [u3;u(9);u4;u(12)]; % Local dispacements of beam2;
    q_loc = [q1;q2];
    
    [fe] = gapElec(params.permittivity, params.l, params.h,... 
       params.gap, params.V, q_loc);
    % derivatives of electrotatic forces on voltage;
    dF2_node1 = zeros(8);   
    dF2_node1(1:3,4) = fe(1:3)*2/params.V;
    dF2_node1(5:7,4) = fe(4:6)*2/params.V;
    R8 = eye(8);
    R8(1:2,1:2) = R;
    R8(5:6,5:6) = R;
    dF11 = R8*dF2_node1*R8';
    dF2 = [dF11 -dF11;
          -dF11  dF11];
    output = output + dF2;   
  end
 
  if length(q)>16
    xdot = q(17:32);
    I1 = -D1*xdot;
    I2 = -D2*xdot;
    % numerically compute dI1/dx(i) and dI2/dx(i);
    delta_x = 1e-9;
    for i=1:16
      x_0 = x(i) - delta_x;
      x_loc = [x(1:(i-1)); x_0; x((i+1):16)];
      [D1_loc, D2_loc] = compute_dFdxdot(R, params, x_loc);
      delta_I1 = I1 + D1_loc*xdot; %= I1 - (- D1_loc*xdot); 
      delta_I2 = I2 + D2_loc*xdot; %= I2 - (- D2_loc*xdot);
      dF3(4,i) = delta_I1 / delta_x;
      dF3(8,i) = delta_I2 / delta_x;
    end
    dF3(12,:) = -dF3(4,:);
    dF3(16,:) = -dF3(8,:);
    output = output + dF3 ;
  end
  
case 'dFdxdot'
  output(4,:) = -D1;
  output(8,:) = -D2;
  output(12,:) = D1;
  output(16,:) = D2;

end
 
% ---

% dFdxdot is only function of x, not xdot in this case;
function [D1, D2] = compute_dFdxdot(R, params, x)
R8 = eye(8);
R8(1:2,1:2) = R;
R8(5:6,5:6) = R;
q1 = R8'*x(1:8);      % Coordinate transformation from global to local
q2 = R8'*x(9:16);
u  = [q1; q2];
d0 = params.gap;

% first derivatives of capacitance due to capacitance change;  
dy1 = u(2) - u(10);
dy2 = u(6) - u(14);
D1 = zeros(1,16);
D2 = zeros(1,16);
v13 = u(4) - u(12);
v24 = u(8) - u(16);

if abs((dy1-dy2)/d0) < 1e-6 % two plates are parallel;
  c13 = params.permittivity * params.h * params.l/2/(d0+dy1);
  D1(2) = v13*(-c13/(d0+dy1));
  D1(4) = c13;
  D1(6) = D1(2);
  
  D2(2) = D1(2);
  D2(6) = D1(2);
  D2(8) = c13; % c13=c24
  
else
  c = params.permittivity * params.h * params.l/(dy2 - dy1);
  const = (dy1+ dy2)/2 + d0;
  c13 = c * (log(const) - log(d0+dy1));
  c24 = c * (log(dy2+d0) - log(const));
  D1(2)  = v13*(-c13/(dy2-dy1) + c * (1/const/2 -1/(d0+dy1)));
  D1(4)  = c13;
  D1(6)  = v13*(c13/(dy2-dy1) + c * (1/const/2));
  
  D2(2)  = v24*(-c24/(dy2-dy1) + c * (-1/const/2 ));
  D2(6)  = v24*(c24/(dy2-dy1) + c * (1/(dy2+d0) - 1/const/2));
  D2(8)  = c24;
end

D1(10) = -D1(2);
D1(12) = -D1(4);
D1(14) = -D1(6);

D2(10) = -D2(2);
D2(14) = -D2(6);
D2(16) = -D2(8); 

D1(1:8)  = D1(1:8)*R8';
D1(9:16) = D1(9:16)*R8';
D2(1:8)  = D2(1:8)*R8';
D2(9:16) = D2(9:16)*R8';

