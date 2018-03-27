function cho_draw_mlab_beam(T, L, W, H, q)

resolution = 20;

 x1 =  q(1);  y1 =  q(2);  z1 =  q(3);
rx1 =  q(4); ry1 =  q(5); rz1 =  q(6);
 x2 =  q(7);  y2 =  q(8);  z2 =  q(9);
rx2 = q(10); ry2 = q(11); rz2 = q(12);

A = [0    0    0    0;
     W/2  W/2 -W/2 -W/2;
    -H/2  H/2  H/2 -H/2];
B = A;

A = rot2local(rx1, ry1, rz1)' * A;
B = rot2local(rx2, ry2, rz2)' * B;

s = linspace(0, L, resolution); 

hx = x1 + (1 + (x2-x1)/L)*s;
hy = hermite_cubic(L, y1,rz1, y2,rz2, s);
hz = hermite_cubic(L, z1,-ry1, z2,-ry2, s);

for corner = 1 : 4

  px = A(1,corner) + (B(1,corner)-A(1,corner))*s/L + hx;
  py = A(2,corner) + (B(2,corner)-A(2,corner))*s/L + hy;
  pz = A(3,corner) + (B(3,corner)-A(3,corner))*s/L + hz;

  p = T(1:3,1:3) * [px; py; pz];

  u(corner,:) = p(1,:) + T(1,4);
  v(corner,:) = p(2,:) + T(2,4);
  w(corner,:) = p(3,:) + T(3,4);
 
end

X = [u(1:4,:); u(1,:)];
Y = [v(1:4,:); v(1,:)];
Z = [w(1:4,:); w(1,:)];

surfl(X,Y,Z); 


function [f] = hermite_cubic(L, f0, f00, fL, fLL, s)

f0L   = (fL - f0)/L;
f00L  = (f0L - f00)/L;
f0LL  = (fLL - f0L)/L;
f00LL = (f0LL - f00L)/L;

f = f0 + s.*(f00 + s.*(f00L + (s-L).*f00LL));

