function [scale] = default_scales(name)

switch (name)

case {'x', 'y', 'z'}
  scale = 1e-6;

case {'rx', 'ry', 'rz'}
  scale = 1e-3;

case 'e'
  scale = 1;

case 'i'
  scale = 1e-3;

otherwise
  scale = 1;

end
