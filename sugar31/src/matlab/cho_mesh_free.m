% Deallocates a netlist structure.
%
% Note that using the Matlab 'clear' command does not deallocate the
% internal data structures used by SUGAR.  Use cho_mesh_free to deallocate
% such structures.  In particular, it is a good idea to use cho_mesh_free
% if you are doing a parameter sweep and creating lots of new netlist
% data structures.
%
% Inputs:
%   net -- a pointer to the netlist structure to be deallocated

function cho_mesh_free(handle)
sugarmex('mesh_free', handle);
