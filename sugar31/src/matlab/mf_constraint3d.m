% Model to constrain nodes in 3D to behave as though they were
% connected to a rigid body.

function [output] = mf_constraint3d(flag, varargin)

output = [];

switch (flag)
  
 case 'init'
  net = varargin{1};
  params.nodes       = cho_mesh_param_nodes(net) ;

  output = params;


 case 'pos'
  % Post-position case

  params = varargin{1};
  net    = varargin{2};
  
  % Here we take the coordinates of Master and connected nodes
  M = cho_mesh_node(net, params.nodes(1));  % -- Master node
  
  ntotal  = length(params.nodes);
  nslaved = ntotal - 1;
  Imaster = 1:6;
  
  T = zeros( 6*nslaved, 6*ntotal );

  for i=1:nslaved
    
    A       = cho_mesh_node(net, params.nodes(i+1));  % -- Slaved node
    delta   = M.x - A.x;
    Islaved = 6*i+1:6*i+6;
   
    rotation = [ 0        -delta(3)  delta(2);
		 delta(3)  0        -delta(1);
		-delta(2)  delta(1)  0       ];
    
    T( Islaved-6, [Imaster, Islaved]) = ...
	[ eye(3),   rotation, -eye(3),    zeros(3) ;
	  zeros(3), eye(3),    zeros(3), -eye(3)  ];
    
  end        

  Tlocal = [ zeros(6*ntotal)   T'               ;
             T                 zeros(6*nslaved) ]; 

  params.T    = Tlocal ;
  output      = params;
  

 case 'vars'
  % Setting up which variables we contribute
  
  params = varargin{1};
  varmgr = varargin{2};  

  % -- Order of variables: variables at A, at B, and then internal

  % displacements and rotations at the boundary
  v = [cho_vars_node(varmgr, params.nodes(1), 'x');
       cho_vars_node(varmgr, params.nodes(1), 'y');
       cho_vars_node(varmgr, params.nodes(1), 'z');
       cho_vars_node(varmgr, params.nodes(1), 'rx');
       cho_vars_node(varmgr, params.nodes(1), 'ry');
       cho_vars_node(varmgr, params.nodes(1), 'rz')];

  for i=1:length(params.nodes)-1
  
    v = [v;
	 cho_vars_node(varmgr, params.nodes(i+1), 'x');
	 cho_vars_node(varmgr, params.nodes(i+1), 'y');
	 cho_vars_node(varmgr, params.nodes(i+1), 'z');
	 cho_vars_node(varmgr, params.nodes(i+1), 'rx');
	 cho_vars_node(varmgr, params.nodes(i+1), 'ry');
	 cho_vars_node(varmgr, params.nodes(i+1), 'rz')];

  end

  % lagrange multiplier
  l1 = 6 * (length(params.nodes)-1);
  
  for i=1:length(params.nodes)-1
  
    v = [v;
	 cho_vars_branch(varmgr, 'lx');
	 cho_vars_branch(varmgr, 'ly');
	 cho_vars_branch(varmgr, 'lz');
	 cho_vars_branch(varmgr, 'lrx');
	 cho_vars_branch(varmgr, 'lry');
	 cho_vars_branch(varmgr, 'lrz')];

  end

  params.vars = v;
  output = params;


 case 'R'
  % Calculate the residual

  params = varargin{1};
  Rvec   = varargin{2};
  xvec   = varargin{3};

  xlocal = cho_x_local(xvec, params.vars);
  Rlocal = params.T * xlocal;

  cho_add_R(Rvec, params.vars, Rlocal);


 case 'dR'
  % Calculate the tangent matrix

  params = varargin{1};
  dRvec  = varargin{2};
  coeff  = varargin{3};
  xvec   = varargin{4};

  cho_add_dR(dRvec, params.vars, coeff(1)*params.T);


 case 'output'
     
  params = varargin{1};
  netout = varargin{2};

  cho_netout(netout, 'nodes', params.nodes);
  cho_netout(netout, 'vars', params.vars);

end
