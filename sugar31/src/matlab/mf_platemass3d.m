% Point mass model

function [output] = mf_platemass3d(flag, varargin)

output = [];

switch (flag)
  
 case 'init'
  net = varargin{1};
  params.node       = cho_mesh_param_nodes(net) ;
  params.material   = cho_mesh_get_param(net, [], 'material');
  l          = cho_mesh_get_param(net, params.material, 'l');
  w          = cho_mesh_get_param(net, params.material, 'w');
  h          = cho_mesh_get_param(net, params.material, 'h');
  density    = cho_mesh_get_param(net, params.material, 'density');
  to_global  = cho_mesh_get_param(net, params.material, 'rot');

  % Translational mass
  params.totalM = l * w * h * density;

  % Integrals needed for reference inertia tensor
  int_xx = l^3 * w   * h   / 12;
  int_yy = l   * w^3 * h   / 12;
  int_zz = l   * w   * h^3 / 12;
  params.J = density * ...
             diag([ int_yy + int_zz, ...
                    int_xx + int_zz, ...
                    int_xx + int_yy ]);

  % Rotate the inertia tensor to global coords
  to_global = reshape(to_global, 3, 4);
  rot       = to_global(1:3, 1:3);
  params.J  = rot * params.J * rot';

  % Mass matrix in global coordinates
  params.M(1:3,1:3) = params.totalM * eye(3);
  params.M(4:6,4:6) = params.J;

  params.l       = l;
  params.w       = w;
  params.h       = h;
  params.density = density;
  params.rot     = rot;

  output = params;


 case 'vars'
  % Setting up which variables we contribute
  
  params = varargin{1};
  varmgr = varargin{2};  

  v = [cho_vars_node(varmgr, params.node(1), 'x');
       cho_vars_node(varmgr, params.node(1), 'y');
       cho_vars_node(varmgr, params.node(1), 'z');
       cho_vars_node(varmgr, params.node(1), 'rx');
       cho_vars_node(varmgr, params.node(1), 'ry');
       cho_vars_node(varmgr, params.node(1), 'rz')];

  params.vars = v;
  output = params;


 case 'R'
  % Calculate the residual

  if ~isempty(varargin{5})
    params = varargin{1};
    Rvec   = varargin{2};
    avec   = varargin{5};

    % Translational inertia
    alocal = cho_x_local(avec, params.vars);
    Rlocal = params.M * alocal;
    cho_add_R(Rvec, params.vars, Rlocal);

    % Rotational inertia
    
  end

 case 'dR'
  % Calculate the tangent matrix

  params = varargin{1};
  dRvec  = varargin{2};
  coeff  = varargin{3};

  cho_add_dR(dRvec, params.vars, coeff(3)*params.M);


 case 'output'
     
  params = varargin{1};
  netout = varargin{2};

  cho_netout(netout, 'node',    params.node);
  cho_netout(netout, 'vars',    params.vars);
  cho_netout(netout, 'l',       params.l);
  cho_netout(netout, 'w',       params.w);
  cho_netout(netout, 'h',       params.h);
  cho_netout(netout, 'density', params.density);
  cho_netout(netout, 'rot',     params.rot);
  
end
