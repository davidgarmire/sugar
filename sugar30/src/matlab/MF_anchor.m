% Mechanical anchor
%
% Input parameters:
%   l, w, h - length, width, and height of the anchor.
%             Currently used for display purposes only.
%
% Nodes and variables:
%   This model takes one node and anchors (grounds) all its
%   position variables (x, y, z, rx, ry, rz)

function [output] = MF_anchor(flag, R, param, q, t, nodes, varargin);

switch(flag)

case 'vars'

  output.ground =  {1 {'x' 'y' 'z' 'rx' 'ry' 'rz'}};

case 'check'

  if (~isfield(param, 'l') | ...
      ~isfield(param, 'w') | ...
      ~isfield(param, 'h'))
    output = 'Missing length, width, or height parameters';
  else
    output = [];
  end

case 'pos'

  output = [0; 0; 0];

case 'abspos'

  if (isfield(param, 'x') & isfield(param, 'y') & isfield(param, 'z'))
    output = [param.x; param.y; param.z];
  else
    output = [];
  end

case 'display'

  q1 = zeros(12,1);
  displaybeam(q1, nodes(1).pos, R, param.l, param.w, param.h);

otherwise

  output = [];

end
