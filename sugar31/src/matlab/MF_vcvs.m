% Voltage-controlled voltage source / amplifier
%
% Input parameters:
%   A - gain
%
% Nodes and variables:
%   There are four nodes, all with voltage variables ('e').
%   Node 1 and 2 provide input voltages; the output voltage
%   at node 3 is A(v2-v1) with respect to the reference voltage
%   at node 4.

function [output] = MF_opamp(flag, R, params, x, t, nodes, varargin);

switch(flag)

case 'vars'

  output.dynamic = {1 {'e'};
                    2 {'e'}
                    3 {'e'}
                    4 {'e'}};

  output.branch = {'i'};

case 'check'

  if (~isfield(params, 'A'))
    output = 'Unspecified gain (A)';
  else
    output = [];
  end

case 'K'

  A = params.A;

  output = [ 0,  0,  0,  0,  0,
             0,  0,  0,  0,  0,
             0,  0,  0,  0,  1;
             0,  0,  0,  0, -1;
             A, -A, -1,  1,  0 ];

otherwise

  output = [];

end
