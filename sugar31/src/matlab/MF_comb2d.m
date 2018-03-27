% 2D comb with N fingers electrical mechanical model;
% DC model;
% Only fringe forces are considered
%     |-----
%     |   -----|
%     |-----   |
%  1  |   -----|   2
% -+--|-----   |---+--     
%     |   -----|
%     |-----   |
%         -----|
% parameter: l = length of the overlap of fingers;
%            L = length of fingers; 
%            permittivity;
%            oz = orientation angle,from node 1 to node 2; 
%            N  = number of fingers of each set;            
%            h  = thickness (defined in process file);
%            gap = distance between fingers;
%            w = width of fingers;
            
% Modifid by Ningning Zhou from v1.1 on 01/08/2001;

function [output] = MF_comb2d(flag, R, params, q, t, nodes, varargin)

switch(flag)

case 'vars'

  output.dynamic = {1 {'x' 'y' 'rz' 'e'} ;
                    2 {'x' 'y' 'rz' 'e'} };


case 'check'

  if (~isfield(params, 'density')       | ...
      ~isfield(params, 'fluid')         | ...
      ~isfield(params, 'viscosity')     | ...
      ~isfield(params, 'Youngsmodulus') | ...
      ~isfield(params, 'permittivity'))
    output = 'Missing parameters typically specified in process file';
  elseif ~isfield(params, 'l')
    output = 'Missing finger overlap length';
  elseif ~isfield(params, 'w')
    output = 'Missing width w';
  elseif ~isfield(params, 'L')
    output = 'Missing finger length L';
  elseif ~isfield(params, 'gap')
    output = 'Missing gap';
  elseif ~isfield(params, 'h') 
    output = 'Missing height';
  elseif ~isfield(params, 'N')
     output = 'Number of fingers of each set';
  elseif ~isfield(params, 'oz')
     output = 'Missing orientation angle';
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
  
case 'M'
  L  = params.L;
  N  = params.N;
  w  = params.w;
  h  = params.h;
  d0 = params.gap;
  density = params.density; 

  % N fingers and N-1 finger links;
  m1 = N*L*h*w*density + (N-1)*2*w*h*d0*density; 
  m2 = m1;
   
  R8 = eye(8);
  R8(1:2,1:2) = R(1:2);
  R8(5:6,5:6) = R(1:2);
  
  M(1,1)=m1;
  M(2,2)=m1;
  M(5,5)=m2;
  M(6,6)=m2;
  M=R8*M*R8';

case 'pos'

  l = params.l;
  L = params.L;
  
  output = R * ...
      [0  2*L-l ;
       0  0 ;
       0  0 ];

case 'display'
  displayComb2d(q, nodes(1).pos, nodes(2).pos, R, params);
 
otherwise

  output = [];
end

% ---   

function [output] = compute_forces(flag, R, params, q, t)
  l  = params.l;
  N  = params.N;
  d0 = params.gap;
  h  = params.h;   
  epson = params.permittivity;
  
  % q(1)--q(3): displacements at node 1;
  % q(4): electrical potential at node 1;
  % q(5)--q(7): displacements at node 2;
  % q(8): electrical potential at node 2;

  % Electrostatic fringe forces
  voltage = q(4) - q(8);    % e1-e2
  coef = N*epson * h / d0;

  %Coordinate transformation from global to local
  u1 = R'*q(1:2);    % Local displacements of node 1
  u2 = R'*q(5:6);    % Local displacements of node 2
  x1=u1(1);
  x2=u2(1);
  u1dot = R'*q(9:10);    % Local displacements of node 1
  u2dot = R'*q(13:14);    % Local displacements of node 2
  x1dot = u1dot(1);
  x2dot = u2dot(1);
  
  e1=q(4);
  e2=q(8);
  e1dot=q(12);
  e2dot=q(16);
  
  switch(flag)

  case 'F'
      
    % Total electrostatic(fringing) forces on two comb nodes; 
    output = zeros(8,1);
    output(1) = coef * voltage^2;
    output(4) = 2*coef*((e1dot-e2dot)*(l+x1-x2)+(e1-e2)*(l+x1dot-x2dot));
    output(5) = -output(1);
    output(8) = -output(4);
    output(1:2) = R*output(1:2); %from local to global
    output(5:6) = R*output(5:6); 
    
  case 'dFdx' 
    output = zeros(8); 
    output(1,4) = 2*coef* voltage;
    output(1,8) = -output(1,4);
    output(4,1) = 2*coef*(e1dot-e2dot);
    output(4,4) = 2*coef*(x1dot-x2dot);
    output(4,5) = -output(4,1);
    output(4,8) = -output(4,5);
    output(5,4) = -output(1,4);
    output(5,8) = -output(1,8);
    output(8,:) = output(4,:);
    
    R8 = eye(8);
    R8(1:2,1:2) = R;
    R8(5:6,5:6) = R;
    output = R8*output*R8'; % From local to global;
    
  case 'dFdxdot'
    

    % Coefficients of the first derivatives;
    output = zeros(8);
    
    output(4,1) = 2*coef*(e1-e2);
    output(4,4) = 2*coef*(l+x1-x2);
    output(4,5) = -output(4,1);
    output(4,8) = -output(4,4);
    output(8,:) = -output(4,:);
        
    % From local to global;
    R8 = eye(8);
    R8(1:2,1:2) = R;
    R8(5:6,5:6) = R;
    output = R8*output*R8';
  end
 
