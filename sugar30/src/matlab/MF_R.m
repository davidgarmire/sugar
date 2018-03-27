% Linear resistor
%
% Input parameter:
%   R - resistance (in ohms) OR
%   G - conductance (in mhos)
%
% Nodes and variables:
%   {'e'} (voltage) at both nodes

function [output] = MF_R(flag, R, params, nodes, x, varargin);

switch(flag)

case 'vars'

  output.dynamic = {1 {'e'};
                    2 {'e'}};

case 'check'

  if (~isfield(params, 'R') & ~isfield(params, 'G'))
    output = 'Unspecified resistance/conductance';
  else
    output = [];
  end

case 'K'

  if (isfield(params, 'R'))
    G = 1/params.R;
  else
    G = params.G;
  end

  output = [ G -G;
            -G  G];

otherwise

  output = [];

end
