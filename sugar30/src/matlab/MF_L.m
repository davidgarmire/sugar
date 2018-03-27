% Linear inductor
%
% Input parameters:
%   L - inductance (in henrys)
%
% Nodes and variables:
%   {'e'} (voltage) at both nodes

function [output] = MF_L1(flag, R, params, x, t, nodes, varargin);

switch(flag)

case 'vars'

  output.dynamic = {1 {'e'};
                    2 {'e'}};

  output.branch = {'i'};

case 'check'

  if (~isfield(params, 'L'))
    output = 'Unspecified inductance';
  else
    output = [];
  end

case 'K'

  output = [ 0,  0,  1;
             0,  0, -1;
             1, -1,  0 ];

case 'D'

  output = [ 0,  0,  0;
             0,  0,  0;
             0,  0, -L ];

otherwise

  output = [];

end
