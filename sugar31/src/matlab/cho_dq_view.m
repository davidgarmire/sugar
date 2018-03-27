% Extract the displacement corresponding to a particular coordinate
% from a displacement vector q (as output by cho_dc), or an array 
% of displacement vectors (as output by cho_ta).
%
% Inputs:
%   dq    - displacement vector or array of displacement vectors
%   net   - netlist structure
%   node  - name of node
%   coord - name of coordinate at indicated node
%
% Output:
%   dqcoord - value (or vector of values) from dq associated
%             with the indicated coordinate.

function [dqcoord] = cho_dq_coord(dq, net, node, coord);

id = lookup_coord(net, node, coord);
if (id == 0)
  dqcoord = [];
else
  s = size(dq);
  if (s(1) == 1 | s(2) == 1)
    dqcoord = dq(id);
  else
    dqcoord = dq(:,id);
  end
end
