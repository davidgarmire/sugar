echo off
%SUGAR Tutorial. Mode analysis demo of 'mirror.net'.

%   Created by Ningning Zhou on 8/2/99;
%   Updated: 

echo on; clc
%                     SUGAR Tutorial - Mode analysis
%##########################################################################
%
%   This is a demonstration script-file for the SUGAR mode analysis
%   
%   It consists of:
%           (2) mode analysis 
%           (3) mode shape display
%
%##########################################################################
pause % Strike any key to display netlist "mirror.net"; 

type mirror.net

pause % Strike any key to display process file "mumps.net"; 

type mumps20.net

pause % Strike any key to load netlist and display the original structure;

clc;
net = cho_load('mirror3.net');  % Load the netlist
cho_display(net);              % Display the original structure
[f, egv, dq] = cho_mode(net);  % Perform mode calculation

pause % Strike any key to display mode shapes

echo off
clc

for mode_num = 1:6
 
  disp(sprintf('Mode number %d', mode_num));
  echo on
  cho_modeshape(net, f, egv, dq, mode_num);
  pause  % Strike any to continue
  echo off
 
end

disp('End of demo')
