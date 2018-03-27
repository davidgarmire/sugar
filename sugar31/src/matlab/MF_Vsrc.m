% Ideal voltage source
%
% Input parameters:
%   V - applied voltage
%
% Nodes and variables:
%   Node 1 is the positive terminal, node 2 is the negative terminal
%   Both nodes have a variable 'e' (voltage).
%   The branch current variable is 'i'.

function [output] = MF_Vsrc(flag, R, params, x, t, nodes, varargin);

switch(flag)

case 'vars'

  output.dynamic = {1 {'e'};
                    2 {'e'}};
  output.branch =  { 'i' }; 


case 'check'

  if (~isfield(params, 'V'))
    output = 'Unspecified voltage V';
  else
    output = [];
  end


case 'K'

  output = [ 0  0  1;
             0  0 -1;
             1 -1  0];


case 'F'

  if ischar(params.V)
    output = [ 0 0 eval(params.V) ]';
  else
    output = [ 0 0 params.V ]';
  end

otherwise

  output = [];

end
