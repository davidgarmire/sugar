% Force/moment function
%
% Input variables:
%   F - applied force (applied in the [1, 0, 0] direction before rotation)
%   M - applied moment (about the [1, 0, 0] axis before rotation)
%
% Nodes and variables:
%   This model takes one mechanical node and applies the indicated force
%   and moment to it.  Variables affected are x, y, z, rx, ry, rz.

function [output] = MF_f3d(flag, R, param, x, t, nodes, varargin)

switch(flag)

case 'vars'

  output.dynamic = {1 {'x' 'y' 'z' 'rx' 'ry' 'rz'}};


case 'check'

  if (~isfield(param, 'F') & ~isfield(param, 'M'))
    output = 'Neither force F nor moment M are specified';
  else
    output = [];
  end


case 'F'

  if isfield(param, 'F') %Force
    F = param.F * R(1:3,1);
  else
    F = [0; 0; 0];
  end

  if isfield(param, 'M') %Moment
    M = param.M * R(1:3,1);
  else
    M = [0;0;0];
  end

  output = [F; M];

otherwise

  output = [];

end

