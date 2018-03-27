function [handle] = cho_load(file, params)
if nargin == 1
  handle = sugarmex('use', file);
else
  handle = sugarmex('use', file, params);
end
