% Ideal current source
%
% Input parameters:
%   I - Input current
%
% Nodes and variables:
%   Node 1 is the positive terminal, node 2 is the negative terminal.
%   Each node has a voltage variable ('e').

function [output] = MF_Isrc(flag, R, params, x, t, nodes, varargin);

switch(flag)

case 'vars'

  output.dynamic = {1 {'e'};
                    2 {'e'}};


case 'check'

  if (~isfield(params, 'I'))
    output = 'Unspecified current I';
  else
    output = [];
  end


case 'F'

  output = [ -params.I, params.I ]';


otherwise

  output = [];

end
