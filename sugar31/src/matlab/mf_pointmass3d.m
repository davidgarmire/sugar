% Point mass model

function [output] = mf_pointmass3d(flag, varargin)

output = [];

switch (flag)
  
 case 'init'
  net = varargin{1};
  params.node       = cho_mesh_param_nodes(net) ;
  params.M          = cho_mesh_get_param(net, [], 'M');

  output = params;


 case 'vars'
  % Setting up which variables we contribute
  
  params = varargin{1};
  varmgr = varargin{2};  

  v = [cho_vars_node(varmgr, params.node(1), 'x');
       cho_vars_node(varmgr, params.node(1), 'y');
       cho_vars_node(varmgr, params.node(1), 'z')];

  params.vars = v;
  output = params;


 case 'R'
  % Calculate the residual

  if ~isempty(varargin{5})
    params = varargin{1};
    Rvec   = varargin{2};
    avec   = varargin{5};

    alocal = cho_x_local(avec, params.vars);
    Rlocal = params.M * alocal;

    cho_add_R(Rvec, params.vars, Rlocal);
  end

 case 'dR'
  % Calculate the tangent matrix

  params = varargin{1};
  dRvec  = varargin{2};
  coeff  = varargin{3};

  cho_add_dR(dRvec, params.vars, coeff(3)*params.M*eye(3));


 case 'output'
     
  params = varargin{1};
  netout = varargin{2};

  cho_netout(netout, 'node', params.node);
  cho_netout(netout, 'vars', params.vars);
  cho_netout(netout, 'M',    params.M);

end
