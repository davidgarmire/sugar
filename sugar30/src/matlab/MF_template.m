% This is a template/example of the proposed SUGAR 2.0 model function layout.
% In particular, I use the 2D electrostatic gap model.
%
% Parameters:
%
%  flag - String specifying what function the caller needs performed
%    == 'vars' - Return the list of ground, dynamic, and branch variables
%                associated with the different nodes in this model.
%                This is almost identical to 'dcdefine' from SUGAR 1.x
%    == 'check' - Check to make sure that all the needed parameters are
%                 present, and that they are valid.
%    == 'M' - Compute the local mass matrix
%    == 'D' - Compute the local damping matrix
%    == 'K' - Compute the local stiffness matrix
%    == 'F' - Compute the right-hand-side function F
%    == 'dF' - Compute the local contribution to the Jacobian matrix dF/dx
%    == 'pos' - Determine relative undisplaced positions of nodes affected by
%               a mechanical element
%    == 'abspos' - Determine absolute undisplaced positions of nodes affected 
%                  by a mechanical element
%    == 'display' - Display this element
%
%    Not every model function need define every case.  If a model function
%    has no particular contribution to make, it can return an empty array
%    ([]).  See, for example, the mechanical anchor model function, which
%    only does anything for the 'vars' case!
%
%  R - Rotation matrix from element local coordinates to global coords
%
%  params - Structure containing parameters to the model function for this
%    element, such as orientation (ox,oy,oz), resistance (R), etc.  This
%    structure will also contain any parameters inherited from the process
%    information.
%
%  x - A state vector.  This should only be used in evaluating F and dF.
%
%  t - Time.  This should only be used in evaluating F and dF.
%
%  nodes - The structures associated with the nodes this element affects.
%    For more information on the node info structure, see the comments
%    in parse_enrich.m.
%
% Output:
%
%  Depends on the flag.  See below for the details for each type.

function [output] = MF_template(flag, R, params, x, t, nodes);

switch(flag)

case 'vars'

  % For the 'vars' flag, the output is a structure which can contain
  % three fields: ground, dynamic, and branch.
  %
  % Ground variables are variables which are always forced to zero, 
  % such as the voltage at an electrical ground, or the displacement 
  % at a mechanical anchor.  Note that the variables which one element
  % considers dynamic may be grounded by another element. 
  %
  % Dynamic variables are nodal variables which appear as free variables in 
  % the equations for this model.
  %
  % Branch variables are free variables associated with the element itself
  % rather than with any particular node.  Examples include the branch
  % current for an inductor.
  %
  % The format of output.dynamic (or output.ground) is
  %   { nodeid1 {'var1' 'var2' ...};
  %     nodeid2 {'var1' ...};
  %      ...
  %   }
  % i.e. it is a cell array of rows, one for each node.  The first entry
  % on a row identifies the number of a node, and the second entry is
  % a cell array containing the names of the associated variables.
  %
  % The format of output.branch is
  %  { 'var1' 'var2' 'var3' ...}
  % where 'var1', etc. are the names of branch variables.
  %
  % If a model contributes no variables of a particular type, then the 
  % corresponding field should not appear in the model function output.
  % For example, this model only contributes dynamic variables.

  output.dynamic = {1 {'x' 'y' 'rz'} ;
                    2 {'x' 'y' 'rz'} ;
                    3 {'x' 'y' 'rz'} ;
                    4 {'x' 'y' 'rz'} };


case 'check'

  % This case may perform whatever sort of parameter checking is appropriate.
  % This function simply checks to ensure that the requisite parameters
  % are present; a more sophisticated routine might check that the parameters
  % are appropriately sized as well (e.g. no negative lengths or widths).
  % If there is an error, a descriptive string is returned via output;
  % otherwise, an empty array is returned.

  if (~isfield(params, 'density')       | ...
      ~isfield(params, 'fluid')         | ...
      ~isfield(params, 'viscosity')     | ...
      ~isfield(params, 'Youngsmodulus') | ...
      ~isfield(params, 'permittivity'))
    output = 'Missing parameters typically specified in process file';
  elseif ~isfield(params, 'l')
    output = 'Missing length';
  elseif ~isfield(params, 'w1')
    output = 'Missing width w1';
  elseif ~isfield(params, 'w2')
    output = 'Missing width w2';
  elseif ~isfield(params, 'gap')
    output = 'Missing gap';
  elseif ~isfield(params, 'h') 
    output = 'Missing height';
  elseif ~isfield(params, 'V1')
    output = 'Missing V1';
  elseif ~isfield(params, 'V2')
    output = 'Missing V2';
  elseif ~isfield(params, 't1')
    output = 'Missing t1';
  elseif ~isfield(params, 't2')
    output = 'Missing t2';
  else
    output = [];
  end


case 'M'

  % Return the local mass matrix.  The order in which the variables are
  % indexed should correspond to the order in which they appear in the
  % 'vars' output, with dynamic variables first, then branch variables.

  output = beam_matrices('M', params); 

case 'D'

  % Return the local damping matrix

  output = beam_matrices('D', params);

case 'K'

  % Return the local stiffness matrix

  output = beam_matrices('K', params);

case 'F'

  % Return the local contribution to the force vector

  [F, dF] = compute_forces(flag, params, x, t);
  output = F;
 
case 'dF'

  % Return the Jacobian of the local contribution to the force vector

  [F, dF] = compute_forces(flag, params, x, t);
  output = dF;

case 'pos'

  % Figure out the relative positions of the different nodes in a 
  % mechanical element.  The jth column gives the relative position
  % of the jth node.  The rows are in the standard order (x,y,z).

  R = rot2local(0, 0, params.oz)';
  l = params.l;
  g = params.gap + (params.w1 + params.w2)/2;

  output = R * ...
      [0  l   0  l;
       0  0  -g -g;
       0  0   0  0];

case 'abspos'

  % If the user gives parameters 'x', 'y', and 'z', it means they have
  % an absolute coordinate in mind for the first node.  If you really
  % wanted, you could have four of these to position all four nodes.

  if (isfield(params, 'x') & isfield(params, 'y') & isfield(params, 'z'))
    output = [params.x; params.y; params.z];
  else
    output = [];
  end

case 'display'

  % Display this element.  Note that the displacements (q) and the
  % undisplaced node positions (nodes(i).pos) are available.
  % The function to display a gap (at the end) just calls the old
  % "displaybeam" routine twice.

  GapDisplay(R, params, x, nodes);

otherwise

  % Please always put an "otherwise" clause at the end of your models.
  % That way, the selection of possible outputs can grow in the future
  % without necessarily requiring that the existing code be modified.

  output = [];

end


% ---

function [M] = beam_matrices(flag, params)

params.w = params.w1;
MB1 = MF_beam2d(flag, params);

params.w = params.w2;
MB2 = MF_beam2d('M', params);

Z = zeros(6);

M = [MB1    Z;
       Z  MB2];


% ---

function [F, dF] = compute_forces(flag, params, x, t)

R = rot2local(params.oz);

x1 = R*x(1:2);    % Local coordinates of node 1
x2 = R*x(4:5);    % Local coordinates of node 2
x3 = R*x(7:8);    % Local coordinates of node 3
x4 = R*x(10:11);  % Local coordinates of node 4

t1 = params.t1;   % Start time
t2 = params.t2;   % End time
V1 = params.V1;   % Start voltage
V2 = params.V2;   % End voltage

gap = params.gap;           % Gap width
l = params.l;               % Length of the beams
A = l * params.h;           % Area of attracting surfaces
e0 = params.permittivity;   % Permittivity of free space (?)

% Current voltage gap
if (t < t1)
  V = V1 * t/t1;
elseif (t < t2)
  V = V1 + (V2-V1)*(t-t1)/(t2-t1);
end

c = e0 * V*V / A;           % Constant in attraction magnitude

% Distance squared between node 1-3 and between node 2-4
d1 = sum(( x1 - x3 + [0; gap] ).^2);  % |x1 - x3|^2
d2 = sum(( x2 - x4 + [0; gap] ).^2);  % |x2 - x4|^2

F1 = [R'*[ 0; -c/(4*d1)] ;   % Force on node 1, y direction
     -c*l/(24*d1)        ] ; % Moment on node 1

F2 = [R'*[ 0; -c/(4*d2)] ;   % Force on node 2, y direction
      c*l/(24*d2)        ] ; % Moment on node2 

% Forces and moments on nodes 3 and 4 are opposite those on 1 and 2
F = [F1; F2; -F1; -F2];

% Partial Jacobians of force/moment on node1 vs x1, y1 (resp node 3, x3, y3)
J11 = -2/d1 * F1 * ([ [x1(1)-x3(1)  x1(2)-x3(2)+gap]*R  0]);
J22 = -2/d1 * F2 * ([ [x2(1)-x4(1)  x2(2)-x4(2)+gap]*R  0]);

% Form the complete Jacobian
Z = zeros(3);
dF = [ J11,    Z, -J11,    Z ;
         Z,  J22,    Z, -J22 ;
      -J11,    Z,  J11,    Z ;
         Z, -J22,    Z,  J22 ];


% ---

function GapDisplay(R,params, q, nodes)

% Put in dummy rotations (since 3D display of 2D model) 
%params.ox = 0;
%params.oy = 0;

% Set up width and displacement (with zero padding for z, rx, ry)
% and display the first beam
params.w = params.w1;
q1([1 2 6 7 8 12]) = q(1:6);
displaybeam(q1, nodes(1).pos, R, params);

% Set up width and displacement (with zero padding for z, rx, ry)
% and display the second beam
params.w = params.w2;
q2([1 2 6 7 8 12]) = q(7:12);
displaybeam(q2, nodes(3).pos, R, params);

