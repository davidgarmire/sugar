% Initialize the SUGAR paths.

addpath([cd '/src/matlab']);
addpath([cd '/src/lua']);
addpath([cd '/src/c']);

try
  sugarmex
catch
  disp(' ');
  disp('Could not access the SUGAR MEX file.  You may not have the file');
  disp('compiled for your platform.  Look at the INSTALL file for directions');
  disp('on how to compile the MEX file.');
  disp(' ');
end
