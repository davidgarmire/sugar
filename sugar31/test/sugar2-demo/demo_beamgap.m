echo off
%SUGAR Tutorial. Electrical-mechanical analysis of beam gap structure.

%   Created by Ningning Zhou on 02/13/2001;
%   Updated: dbindel, 9/8/2001

echo on; clc
%                     SUGAR Tutorial - Static Analysis
%##########################################################################
%   Electrical-mechanical analysis of beam gap structure.%
%##########################################################################

pause % Strike any key to display netlist "beamgap2e.net"; 

type beamgap2e.net

pause % Strike any key to display process file "mumps.net"; 

type mumps20.net

pause % Strike any key to load netlist and display the original structure;

net = cho_load('beamgap2e3.net');

figure;
cho_display(net);

pause % Strike any key to continue
clc

%########################################################################
%
%   Static Analysis;
%
%#######################################################################

pause % Strike any key to analyze and display deflection at 10V

[dq] = cho_dc(net);
figure;
cho_display(net,dq);
title('Deflected structure at V=10v');

pause % Strike any key to exit
echo off
disp('End of demo')
