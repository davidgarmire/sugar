% Adds to the netlist search path.
%
% When netlists are accessed via cho_load or via a 'use' command inside
% another netlist, SUGAR searches in the current directory and then in
% the directories on the netlist search path.  cho_addpath adds a new
% directory to the netlist search path.
%
% Inputs:
%   path -- name of the file system path to be added.

function cho_addpath(path)
sugarmex('addpath', path);
