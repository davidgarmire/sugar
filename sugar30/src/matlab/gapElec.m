function [fk] = gapElec(epson, L,thick,d0,voltage,u)
% Gap electrostatic forces only on nodel 1,2. Model level 1; 
% Equilibrate the distributed electrostatic forces on four nodes
%   of the gap; Fringe forces are zeros. 
% Forces on node 3, 4 are equal and opposite to that on node 1, 2.
% Use linear approximation for the shape of gap beams;
 
% Created by Ningning Zhou on 03/09/98; latest modified on 10/4/2000

%% u(1,2,3)----node 1;          u(4,5,6)---node 2;
%% u(7,8,9)--- node 3;          u(10,11,12)----node 4;

%epson=8.854187817e-12 for air;
g1= epson * voltage^2 * thick/2;

fk= zeros(6,1);

v13 = u(2) - u(8);
v24 = u(5) - u(11);

b = v13 + d0;              % gap distance between node 1 and 3; 
c = v24 + d0;              % gap distance between node 2 and 4;
a = (c-b) / L;             % a=0 ==> parallel gap 

%%
%% fk(), external forces applied on node1,2 3 4
%% fk(1,2,3)---node 1;   fk(4,5,6)---node 2;

x = (c - b)/b;
scl1 = - g1 * L;
scl2 = scl1 * L;
if (abs(x) < 0.2) 
     tx    =   taylor(x);
     fk(2) =   6 * scl1 * tx/b^2;
     fk(3) =   scl2 * ((3 + x) * tx - 1.0/6)/b^2;
     y     =   (b - c)/c;
     ty    =   taylor(y);
     fk(5) =   6 * scl1 * ty/c^2;
     fk(6) = - scl2 * ((3 + y) * ty - 1.0/6)/c^2;
else
     fk(2) =   scl1 * integ_1(b, c);
     fk(3) =   scl2 * integ_2(b, c);
     fk(5) =   scl1 * integ_1(c, b);
     fk(6) = - scl2 * integ_2(c, b);
end
%  fk(1)= g1/(d0 + u(5) - u(11));             % fringe force(n2) on plate 1-2;
%  fk(4)= -g1/(d0 + u(2) - u(8));             % fringe force(n3) on plate 3-4;

alpha = 1 + d0/thick * 1.12;   
fk = alpha*fk;
% 
% Taylor expansion for the function 
%     (x^2 - 3x - 6)/x^3/6 + (1+x)/x^4 log(1 + x)
% near x = 0. optional parameter tol is tolerance for truncation error.
% Default is machine precision.

function [fn] = taylor(x, tol)
%
if (nargin == 1)
     tolerance = eps;
else
     if (tol <= 0)
         tolerance = eps;
     else
        tolerance = eps;
     end
end
if (abs(x) < 0.25) 
   k = ceil(- log(tolerance)/log(4)) - 1;
   fn = 1.0/((k+3) * (k + 4));
   for j = k-1:-1:0
       fn = 1.0/((j+3) * (j + 4)) - x * fn;
   end
else
   fn = ( (x^2 - 3*x - 6)/6 + (1+x) * log(1 + x)/x)/x^3;
end

%
% This is the integral for fk(2) and fk(5)
%
function [fn] = integ_1(b, c)

fn =( (c^2 - 5 * b * c - 2 * b^2)/b + 6 * b * c/(c - b) * log(c/b))/(c-b)^3;
%
% This is the integral for fk(3) and fk(6)
%
function [fn] = integ_2(b, c)

fn = ( -0.5 * (b + 5 * c) + c* (c + 2 * b)/(c-b) * log(c/b) )/(c-b)^3;

