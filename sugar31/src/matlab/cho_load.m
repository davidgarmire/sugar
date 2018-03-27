% Load a netlist description into the SUGAR internal format.
%
% Inputs: 
%   netname - netlist file name
%   param   - (Optional) parameter settings
%
% Outputs:
%   net - pointer to netlist structure

function [handle] = cho_load(file, params)
if nargin == 1
  handle = sugarmex('use', file);
else
  handle = sugarmex('use', file, params);
end
