function cho_mdraw(mesh, x)

clf;
grid on;
hold on;
view(0,90);
xlabel('X - horizontal  [m]');
ylabel('Y - vertical  [m]');
zlabel('Z - out of plane  [m]');

if nargin == 1
    sugarmex('mdraw', mesh);
else
    sugarmex('mdraw', mesh, x);
end    

rotate3d on;
color=pink;
color(:,2)=color(:,2)*(0.5 + 0.5*rand);
color(:,1)=color(:,1)*(0.5 + 0.5*rand);
color(:,3)=color(:,3)*(0.5 + 0.5*rand);
colormap(color);
shading interp;
axis equal;
axis vis3d;
hold off;

