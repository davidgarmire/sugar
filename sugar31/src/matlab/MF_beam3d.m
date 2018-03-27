% 3D small-deflection linear beam model
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
%   the usual spatial displacement variables (x, y, z, rx, ry, rz)

function [output] = MF_beam3d(flag, R, param, q, t, nodes, varargin);

switch(flag)

case 'vars'

  output.dynamic = {1 {'x' 'y' 'z' 'rx' 'ry' 'rz'};
                    2 {'x' 'y' 'z' 'rx' 'ry' 'rz'}};

case 'check'

  if (~isfield(param, 'density')       | ...
      ~isfield(param, 'fluid')         | ...
      ~isfield(param, 'viscosity')     | ...
      ~isfield(param, 'Youngsmodulus'))
    output = 'Missing param typically specified in process file';
  elseif ~isfield(param, 'l')
    output = 'Missing length';
  elseif ~isfield(param, 'w')
    output = 'Missing width';
  elseif ~isfield(param, 'h') 
    output = 'Missing height (layer thickness)';
  else
    output = [];
  end


case 'M'
   
   %geometrical param
   H = param.h;               %Beam layer thickness, along z-axis.
   W = param.w;               %Beam width, along y-axis.
   L = param.l;               %Beam length, along x-axis.
   D = param.density;         %Poly-Si density.
   A = H * W;                 %Beam crossectional area, yz-plane.
   Iy = W * H^3 / 12;         %Rotatory moment of inertia about y.
   Iz =	H * W^3 / 12;         %Rotatory moment of inertia about z. Used in 2D.
   Jx =	Iy + Iz;              %Polar moment of inertia, torsional inertia.
                              %J=0.14 if h<=w, cJ=1/3 if h<<w.
   A = H * W;					%Cross sectional area.   
   a =  1 / 3;					%Various quantities...
   b = 13 / 35;
   c = 13 / 35;
   d = Jx / (3 * A);
   e = L^2 / 105;
   f = L^2 / 105;
   g = 1 / 3;
   h = 13 / 35;
   i = 13 / 35;
   j = Jx / (3 * A);
   k = L^2 / 105;
   l = L^2 / 105;
   m = -11 * L / 210;
   n = 11 * L / 210;
   oo= 	9 / 70;
   p = 13 * L / 420;
   q = 9 / 70;
   r = -13 * L / 420;
   s = Jx / (6 * A);
   t = -13 * L / 420;
   u = -L^2 / 140;
   v = 1 / 6;
   w = -L^2 / 140;
   
%rotation matrix
   Rall(1:3,1:3) = R;
   Rall(4:6,4:6) = R;
   R = Rall;
  	 
%mass matrix 
  
   M11 = R * [ ...
     a     0     0     0     0     0;
     0     b     0     0     0     n;
     0     0     c     0     m     0;
     0     0     0     d     0     0;
     0     0     m     0     e     0;     
     0     n     0     0     0     f] * R';
   M22 = R * [ ...
     g     0     0     0     0     0;
     0     h     0     0     0    -n;
     0     0     i     0    -m     0;
     0     0     0     j     0     0;
     0     0    -m     0     k     0;
     0    -n     0     0     0     l] * R';
   M21 = R * [ ...
     v     0     0     0     0     0;
     0     oo    0     0     0     p;
     0     0     q     0     r     0;
     0     0     0     s     0     0;
     0     0    -r     0     w     0;
     0     t     0     0     0     u] * R';
  
   output = D*A*L*[M11  M21'; M21 M22];
  
case 'D'

%matrix param
   H = param.h;		%Beam layer thickness, along z-axis.
   W = param.w;		%Beam width, along y-axis.
   L = param.l;		%Beam length, along x-axis.
   A = H * W;		%Beam crossectional area, yz-plane.
   Mu = param.viscosity;%Viscocity of fluid environment.
   delta = param.fluid;	%Fluid layer thickness.
   Iy =	W * H^3 / 12;	%Rotatory moment of inertia about y.
   Iz =	H * W^3 / 12;	%Rotatory moment of inertia about z. Used in 2D.
   Jx =	Iy + Iz;	%Polar moment of inertia, torsional inertia.
                        %J=0.14 if h<=w, cJ=1/3 if h<<w.
   A = H * W;					%Cross sectional area.   
   a = 1 / 3;					%Various quantities...
   b = 13 / 35;
   c = 13 / 35;
   d = Jx / (3 * A);
   e = L^2 / 105;
   f = L^2 / 105;
   g = 1 / 3;
   h = 13 / 35;
   i = 13 / 35;
   j = Jx / (3 * A);
   k = L^2 / 105;
   l = L^2 / 105;
   m = -11 * L / 210;
   n = 11 * L / 210;
   oo = 9 / 70;
   p = 13 * L / 420;
   q = 9 / 70;
   r = -13 * L / 420;
   s = Jx / (6 * A);
   t = -13 * L / 420;
   u = -L^2 / 140;
   v = 1 / 6;
   w = -L^2 / 140;
   
%rotation matrix
   Rall(1:3,1:3) = R;
   Rall(4:6,4:6) = R;
   R = Rall;
   
%damping matrix. This viscous model is good for planar motion.  
   D11 = R * [ ...
     a     0     0     0     0     0;
     0     b     0     0     0     n;
     0     0     c     0     m     0;
     0     0     0     d     0     0;
     0     0     m     0     e     0;     
     0     n     0     0     0     f] * R';
   D22 = R * [ ...
     g     0     0     0     0     0;
     0     h     0     0     0    -n;
     0     0     i     0    -m     0;
     0     0     0     j     0     0;
     0     0    -m     0     k     0;
     0    -n     0     0     0     l] * R';
   D21 = R * [ ...
     v     0     0     0     0     0;
     0     oo    0     0     0     p;
     0     0     q     0     r     0;
     0     0     0     s     0     0;
     0     0    -r     0     w     0;
     0     t     0     0     0     u] * R';
  
  output = Mu*W*L/delta*[D11  D21'; D21 D22];

case 'K'

%matrix param
   E = param.Youngsmodulus;	%Modulus of elasticity.
   H = param.h;			%Beam layer thickness, along z-axis.
   W = param.w; 		%Beam width, along y-axis.
   L = param.l;			%Beam length, along x-axis.
   A = H * W;			%Beam crossectional area, yz-plane.

   Nu = param.Poisson;		%Poisson's ratio.
   if isfield(param,'shearmodulus')
      G=param.shearmodulus;
   else
      G=E/(2*(1+Nu));	%Shear modulus. GJ = Torsional stiffness 
   end
   
   if (W>H)
     J = W*H^3*(16/3-3.36*H/W*(1-(H/W)^4/12))/16;
   else
     J = H*W^3*(16/3-3.36*W/H*(1-(W/H)^4/12))/16;
   end

%   %Torsional stiffness (Demeter G. Fertis 1996 p78.)
%   a=W/2;
%   b=H/2;
%   sum=0;
%   for n=0:100
%      kn=(2*n+1)*pi/2/a;
%      sum=sum+tanh(kn*b)/(2*n+1)^5;
%   end
%   J=16*a^3*b*(1/3-(64*a/pi^5/b)*sum); 

   Iy =	W * H^3 / 12;		%Moment about y.
   Iz =	H * W^3 / 12;		%Moment about z. Used in 2D.

   a = E * A / L;
   b = 12 * E * Iz / L^3;
   c = 12 * E * Iy / L^3;
   d = G * J / L;
   e = 4 * E * Iy / L;
   f = 2 * E * Iy / L;
   g = 4 * E * Iz / L;
   h = 2 * E * Iz / L;
   i = 6 * E * Iy / L^2;
   j = 6 * E * Iz / L^2;
   
%rotation matrix
   Rall(1:3,1:3) = R;
   Rall(4:6,4:6) = R;
   R = Rall;
   
   
%shear effects (J.S.Przemieniecki p79)
%   phiy=(1+12*E*Iz/G/L^2/(L*H));
%   phiz=(1+12*E*Iy/G/L^2/(L*W));  
%   b=b/phiy;
%   c=c/phiz;
%   e=e/phiz;
%   g=g/phiy;
%   j=j/phiy;
%   i=i/phiz;
%   f=f/phiz;
%   h=h/phiy;
   
   
   
%stiffness matrix, linear model
   K11 = R * [ ...
     a     0     0     0     0     0; 
     0     b     0     0     0     j; 
     0     0     c     0    -i     0; 
     0     0     0     d     0     0; 
     0     0    -i     0     e     0;     
     0     j     0     0     0     g] * R';
   K22 = R * [ ...
     a     0     0     0     0     0; 
     0     b     0     0     0    -j; 
     0     0     c     0     i     0; 
     0     0     0     d     0     0; 
     0     0     i     0     e     0; 
     0    -j     0     0     0     g] * R';   
   K12 = R * [ ...         
    -a     0     0     0     0     0; 
     0    -b     0     0     0     j; 
     0     0    -c     0    -i     0; 
     0     0     0    -d     0     0; 
     0     0     i     0     f     0;     
     0     -j    0     0     0     h] * R';
  
   output = [K11  K12; K12' K22];


case 'pos'

  % Compute relative positions of beam nodes

  if (isfield(param, 'l'))
    output = R * [0 param.l;
                  0 0;
                  0 0];
  else
    output = [];
  end
  
case 'postpos'

  [params, R] = beam_postpos(param, R, [nodes(1).pos, nodes(2).pos]);
  output.params = params;
  output.R = R;

case 'F'

% Need dF to go with it.

   w = param.w; %width
   h = param.h; %layer thickness
%   r = rot2local(param.ox,param.oy,param.oz);
   r = R';
   o = zeros(3,3);
   R = [r o; o r]; %rotation
   A = w*h; %cross-sectional area
   sigma = param.stress; %Stress < 0 if compressive, > 0 tensile.
   gamma = param.straingradient; %Strain gradient < 0 concave down, > 0 concave up.
   alpha = param.thermalexpansion; %elemental coeffiecient of thermal expansion
   I = h^3*w/12; %Moment of inertial.
   E = param.Youngsmodulus; %Young's modulus.
   ambient = param.ambienttemperature; %Average temperature outside beam
   if isfield(param,'T')
     T = param.T; %Average beam temperature
   else
     T = ambient;
   end
   thermalstress = alpha * E * (ambient-T); %Thermalstress < 0 if compressive, > 0 tensile.
   F_axialstress = sigma * A;
   F_straingradient = E * I * gamma;
   F_thermal = thermalstress * A; 
   F_node1 = R' * [(F_axialstress+F_thermal); 0; 0; 0; F_straingradient; 0];
%   FInertial=FTranslational(m,param.accel)+FCoriolis(m,param.omega,param.vel)+FTransverse(m,param.omegadot,[nodes(1).pos;nodes(2).pos])+FCentrifugal(m,param.omega,[nodes(1).pos;nodes(2).pos]);
   
   output = [F_node1; -F_node1];
   
case 'display'

  displaybeam(q, nodes(1).pos, R, param.l, param.w, param.h);
  
otherwise

  output = [];

end

