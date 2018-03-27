echo off
% Copyright (c) 1999-2000 The Regents of the University of California.
% All rights reserved.
% Permission is hereby granted, without written agreement and without
% license or royalty fees, to use, copy, modify, and distribute this
% software and its documentation for any purpose, provided that the above
% copyright notice and the following two paragraphs appear in all copies
% of this software.
%
% IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
% FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
% ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
% THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
% SUCH DAMAGE.
%
% THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
% INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
% MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
% PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
% CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
% ENHANCEMENTS, OR MODIFICATIONS.

%SUGAR Tutorial. Steady state analysis demo.

%   Created by Ningning Zhou on 02/14/2001

echo on; clc
%                   SUGAR Tutorial - Steady state analysis
%##########################################################################
%
%   This is a demonstration script-file for the SUGAR steady state analysis
%   of a 2D resonator.
%   
%##########################################################################
pause % Strike any key to display netlist "multimode_m.net"; 

type multimode_m.net

pause % Strike any key to display process file "mumps.net"; 

type mumps20.net

%#################################################################
pause % Strike any key to load netlist and display the original structure;

net = cho_load('multimode_m3.net');

pause % Strike any key to display the original structure; 
figure;
cho_display(net);

pause % Strike any key to continue
clc

%########################################################################
%
%      Steady state analysis and bode plot for a multimode resonator
%
%######################################################################
% Perform steady state analysis at a DC equilibrium point; 
pause % Strike any key to continue

% Find a DC equilibrium point;

dq = cho_dc(net);

% Steady state analysis and bode plot;
% Specify the input excitation at 'node10' along 'y' direction;  
% Specify the output response of 'y' displacement of the mass at 'node14';  

figure;
cho_ss(net,dq,'n10','y','n14','y');
