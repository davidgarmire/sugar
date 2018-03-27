echo off
%Sugar tutorial of static and modal analysis of a tang-like resonator.
%by jvclark, March2001

%intro page
clc
fprintf([ ...
 ' SUGAR demo - Static and modal analysis of a Tang-like resonator.\n' ...
 ' This demo consists of:\n' ...
 '   1) Static analysis: displacement & effective stiffness\n' ...
 '   2) Modal analysis: frequency of mode 1 & 5\n' ...
 ' Open file tang.net to see the netlist \n\n' ...
 ' Press key to continue.  ']); 
pause

%static analysis page
clc
fprintf('\n\n');
fprintf(' Static analysis for a lateral force applied on main mass.\n\n');
fprintf(' These Matlab commands will load the netlist, then calculate its deflection:\n\n\n');
fprintf(' net = cho_load(''tang.net'');\n\n');
fprintf(' dq = cho_dc(net);\n\n\n');
fprintf('\n\n\n\n\n Press key to continue.  '); pause
fprintf('\n');
echo on;
net=cho_load('tang3.net');
dq=cho_dc(net);
echo off;

clc
fprintf('\n\n');
fprintf(' Display structure.\n\n');
fprintf(' Use these commands to display the structure before and after applied forces:\n\n\n');
fprintf(' figure(1);cho_display(net);\n\n');
fprintf(' figure(2);cho_display(net,dq);\n\n\n\n');
fprintf(' To rotate the display:\n');
fprintf(' position the mouse over the figure. Then simultaneously left-click and drag the mouse.\n\n\n');
fprintf('\n\n\n\n\n Press key to continue.  '); pause
fprintf('\n');
echo on;
figure(1);cho_display(net);
figure(2);cho_display(net,dq);
echo off;
clc;

%Displacement value page
clc
fprintf('\n\n');
fprintf(' Displacement and effective stiffness values.\n\n');
fprintf(' The following commands will give the displacement value of a particular node & coordinate (x,y,z,rx,ry,rz):\n\n\n');
fprintf(' dx = dq(lookup_coord(net,''j'',''x'')) in [meters]\n');
fprintf(' Kx=30e-6/dx in [N/m]\n\n');
fprintf(' where a force of -30uN is applied at node u.\n\n');
fprintf(' Note: Though netlist angles are degrees, output displacement angles of this function are radians. xyz displacements are meters.\n\n');
fprintf('\n\n\n\n\n Press key to continue.  '); pause
fprintf('\n\n\n\n\n');
echo on;
dx=dq(lookup_coord(net,'j','x'))
Kx=30e-6/dx
echo off;

%Modal analysis
clc
fprintf('\n\n');
fprintf(' Modal analysis of structure.\n\n');
fprintf(' The following commands will give the mode 1 & 5 shapes and frequencies:\n\n\n');
fprintf(' net=cho_load(''tang2.net'');mode=1;magfactor=0.1;[freq,egv,q0]=cho_mode(net);figure(3);cho_modeshape(net,freq,egv,q0,magfactor,mode);\n\n\n');
fprintf(' mode=5;magfactor=1;[freq,egv,q0]=cho_mode(net);figure(4);cho_modeshape(net,freq,egv,q0,magfactor,mode);\n');
fprintf('\n\n\n\n\n Press key to continue.  '); pause
fprintf('\n\n\n\n\n');
echo on;

params.forcesize = 0;
net=cho_load('tang3.net', params);

mode=1;
magfactor=0.06;
[freq,egv,q0]=cho_mode(net);
figure(3);
cho_modeshape(net,freq,egv,q0,magfactor,mode);

mode=5;
magfactor=1;
[freq,egv,q0]=cho_mode(net);
figure(4);
cho_modeshape(net,freq,egv,q0,magfactor,mode);

echo off;
fprintf('\nEnd of demo\n');
