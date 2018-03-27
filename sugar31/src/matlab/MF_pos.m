% Set relative position of two nodes
%
% Input parameters:
%   x, y, z - coordinates of the second node in a frame where
%             the first node is the origin

function [output] = MF_pos(flag, R, param, q, t, nodes, varargin);

switch(flag)

case 'pos'

  % Compute relative positions of nodes

  if (isfield(param, 'x'))
    x = param.x;
  else
    x = 0;
  end

  if (isfield(param, 'y'))
    y = param.y;
  else
    y = 0;
  end

  if (isfield(param, 'z'))
    z = param.z;
  else
    z = 0;
  end

  output = R * [0 x;
                0 y;
                0 z];            

otherwise

  output = [];

end

