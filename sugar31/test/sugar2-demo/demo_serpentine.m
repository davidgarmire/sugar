echo off
%Sugar tutorial of static and modal analysis of a serpentine resonator.
%by jvclark, March2001

%intro page
clc
fprintf('\n\n');
fprintf(' SUGAR demo - Modal analysis of a serpentine resonator using subnets and parameterized netlists.\n\n\n');
fprintf(' This demo consists of:\n\n');
fprintf('   1) Modal analysis\n\n');
fprintf('\n\n\n Open file serpA1.net and serpA1meander1.m to see the netlist and subnet\n\n\n Press key to continue.  '); pause

%Modal analysis
clc
fprintf('\n\n');
fprintf(' Modal analysis of structure.\n\n');
fprintf(' The following commands will give the mode shape and frequency:\n\n\n');
fprintf(' Netlists can be parameterized. E.g. we''ll parameterize a change in width and lenth by \n\n');
fprintf(' param.dW=0e-6;  param.dL=0e-6;    <----- modify netlist parameters here\n\n');
fprintf(' Now we''ll load the netlist and show the modes\n\n');
fprintf(' net=cho_load(''serpA1.net'',param);   <----- Note how param is an input now\n\n');
fprintf(' mode=1;magfactor=0.1;[freq,egv,q0]=cho_mode(net);figure(1);cho_modeshape(net,freq,egv,q0,magfactor,mode);\n');
fprintf(' \nThis analysis will take about a minute since it has many elements');
fprintf('\n\n\n\n\n Press key to continue.  '); pause
fprintf('\n\n\n\n\n');
echo on;

param.dW=0e-6;  
param.dL=0e-6;
net=cho_load('serpA13.net',param);
mode=1;
magfactor=0.1;
[freq,egv,q0]=cho_mode(net,10);
figure(1);
cho_modeshape(net,freq,egv,q0,magfactor,mode);

echo off;
fprintf('\nEnd of demo\n');
