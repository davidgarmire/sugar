% Electrical ground
%
% Input parameters:
%   none
%
% Nodes and variables:
%   This model takes one node and grounds its voltage (e)


function [output] = MF_eground(flag, R, parameters, x, t, varargin);

switch(flag)

case 'vars'

  output.ground =  {1 {'e'}};


otherwise

  output = [];

end
