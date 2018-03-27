function [output] = MF_f2d(flag, R, params, x, t, nodes, varargin)

switch(flag)

case 'vars'

  output.dynamic = {1 {'x' 'y' 'rz'}};


case 'check'

  if (~isfield(params, 'F') & ~isfield(params, 'M'))
    output = 'Neither force F nor moment M are specified';
  else
    output = [];
  end


case 'F'

  if isfield(params, 'F')
    F = params.F * R(1:2, 1);
  else
    F = [0; 0];
  end

  if isfield(params, 'M')
    M = params.M;
  else
    M = [0];
  end

  output = [ F ; M ];
  
otherwise

  output = [];

end
