% Gap contact forces model(1/d^3)(Newton-Raphson limiting);
% recommended limit is limit=0.055e-6 (m);
% Contact forces are only on node 1 and 2; 
% Forces on node 3, 4 are equal and opposite to that on node 1, 2

% Created by Ningning Zhou on 03/09/98; latest modified on 11/6/00;

function [fk] = gapCont(gap, h, u, varargin)

%%
%% u(1,2,3)----node 1;          u(4,5,6)---node 2;
%% u(7,8,9)--- node 3;          u(10,11,12)----node 4;
%%

fk= zeros(6,1);

d13 = u(2) - u(8)  + gap;    % gap distance between node 1 and 3; 
d24 = u(5) - u(11) + gap;    % gap distance between node 2 and 4;

if( d13 > 0.1e-6 )           % Contact forces arenot turned on;
    f13 = 0;
else              % contact forces on,1/d^3 contact forces
    % f13= 0.008N, f'=-4.8e+6 at d13=0.055e-6, set 0.055e-6 as the d13 limit.
    f13 = 1e-9/(-0.05+(d13)*1e6)^3 - 8e-6;
end

f13 = f13/2e-6 * h;      % take into account the thickness effect;

if( d24 > 0.1e-6 )
    f24 = 0;
else     
    % f24= 0.008N, f'= -4.8e+6 at d24=0.055e-6, set 0.055e-6 as the d24 limit.
    % f24=0.037N, f'=-3.4e7 at d24=0.053e-6,
    f24 = 1e-9/(-0.05+(d24)*1e6)^3 - 8e-6;
end

f24 = f24 / 2e-6 * h;      % take into account the thickness effect

%% fk(), external forces applied on node1,2; 
%% fk(1,2,3)---node 1;   fk(4,5,6)---node 2;

fk(2) = f13;
fk(5) = f24;
 
