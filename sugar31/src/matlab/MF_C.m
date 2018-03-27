% Linear capacitor model
%
% This is a standard linear capacitor model:
%   i = C dv/dt
%
% Input parameters:
%   C - the capacitance (in farads).
%
% Nodes and variables:
%   {'e'} (voltage) at both nodes

function [output] = MF_C(flag, R, params, x, t, nodes, varargin);

switch(flag)

case 'vars'

  output.dynamic = {1 {'e'};
                    2 {'e'}};

case 'check'

  if (~isfield(params, 'C'))
    output = 'Unspecified capacitance';
  else
    output = [];
  end

case 'D'

  C = params.C;

  output = [ C -C;
            -C  C];

otherwise

  output = [];

end
