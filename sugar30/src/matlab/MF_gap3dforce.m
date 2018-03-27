%MF_gap3de(option,R,params,q,t,nodes,varargin)

%This is a level 2 model of a gap closing actuator.
%It inherits F dFdx and dFdxdot from the gap2de modelfunction.

%See also: MF_gap2de, MF_gap2d, MF_beam3de
%%coded by jvclark - ucberkeley (spring/2001)

function [output] = MF_gap3dforce(option,R,params,q,t,nodes,varargin)

switch(option)

case 'vars'

   output.dynamic = ... %node variables
     { 1 {'x','y','z','rx','ry','rz','e'};
       2 {'x','y','z','rx','ry','rz','e'};
       3 {'x','y','z','rx','ry','rz','e'};
       4 {'x','y','z','rx','ry','rz','e'}};

case 'check'
  output = MF_gap2dforce(option,R,params); %netlist syntax 

case 'F', %convert gap2d to 3d

  % Get the transformation from 3d global -> 2d local
  T = to_2d_local(R);   
  p = params;

  % Transform into local 2D coordinates; compute forces; and transform back
  q2d = kron(eye(8), T) * q;
  F2d_local = MF_gap2dforce('F', eye(3), p,q2d,t,[],varargin);
  output = kron(eye(4), T') * F2d_local;

case 'dFdx' %convert gap2d to 3d

  % Get the transformation from 3d global -> 2d local
  T = to_2d_local(R);   
  p = params;

  % Transform into local 2D coordinates; compute Jacobian; and transform back
  q2d = kron(eye(8), T) * q;
  dFdx2dloc = MF_gap2dforce(option,eye(3),p,q2d,t,[],varargin);
  output = kron(eye(4), T') * dFdx2dloc * kron(eye(4), T);

case 'dFdxdot' %convert gap2d to 3d

  % Get the transformation from 3d global -> 2d local
  T = to_2d_local(R);   
  p = params;

  % Transform into local 2D coordinates; compute Jacobian; and transform back
  q2d = kron(eye(8), T) * q;
  dFdx2dloc = MF_gap2dforce(option,eye(3),p,q2d,t,[],varargin);
  output = kron(eye(4), T') * dFdx2dloc * kron(eye(4), T);

case 'pos' %global coordinate positions

  l = params.l; 
  g = params.gap+(params.w1+params.w2)/2; %some params

  %global postion coordinates
  output=R* ...
     [0,  l,  0,  l;
      0,  0, -g, -g;
      0,  0,  0,  0];

otherwise
  output = [];

end %switch

%-----------------------------------------------------------
% Form the matrix to rotate to the local 3D coordinates
% and then project down, i.e.
%   T = P * [R' 0  0; 
%            0  R' 0; 
%            0  0  1]
% where R is the rotation from local to global coordinates
% and
%        xyz    rxyz   e
%   P = [1 0 0  0 0 0  0     x
%        0 1 0  0 0 0  0     y
%        0 0 0  0 0 1  0     rz
%        0 0 0  0 0 0  1 ];  e

function [T] = to_2d_local(R)

R = R';
T(1:2, 1:3) = R(1:2, 1:3);
T(3,   4:6) = R(3,   1:3);
T(4,   7  ) = 1;

