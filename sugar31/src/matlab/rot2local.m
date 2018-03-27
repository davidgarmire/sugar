%[r]=rotate2local_yzx_3x3(x,y,z) returns a 3x3 pure rotation matrix. 
%It rotates about the coincident y first, the new z second, then 
%the new x axis.
%This order is equivalent to the direction cosine resulting matrix.
%Local = R * Global

function [R] = rot2local(x,y,z)
%By Jason Vaughn Clark - June1999

if (nargin == 1)
  cx = cos(x); sx = sin(x);
  R = [cx sx;
      -sx cx];
  return;
end

%transformation matrix elements.
cy = cos(y); sy = sin(y);
cz = cos(z); sz = sin(z);
cx = cos(x); sx = sin(x);

%rotation about x-axis.
rx = ...
    [  1   0   0;
       0   cx  sx;
       0 (-sx) cx ];

%rotation about y-axis.
rz = ...
    [   cz  sz  0;
      (-sz) cz  0;
        0   0   1 ];

%rotation about z-axis.
ry = ...
    [  cy  0 (-sy);
       0   1   0;
       sy  0   cy ];

%apply rotations.
R = rx * rz * ry;   
