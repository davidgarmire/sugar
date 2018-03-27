function [result] = mf2wrap(flag, varargin)

switch (flag)

 case 'init'

  result       = varargin{1};
  
  [errmsg] = feval(result.wrapmf, 'check', result.rot, result);
  if ~isempty(errmsg)
    error(errmsg);
  end
  
  result.mf2vars  = feval(result.wrapmf, 'vars');
  result.vars     = [];
  result.bcvars   = [];
  result.nodespos = [];  

 case 'postpos'

  result = varargin{1};
  postpos = feval(result.wrapmf, 'postpos', result.rot, result, [], [], ...
                  result.nodespos);

  if ~isempty(postpos)
    result     = postpos.params;
    result.rot = postpos.R;
  end
 
 case 'R'
    
  param      = varargin{1};
  assemble_R = varargin{2};
  
  xlocal = [];
  vlocal = [];
  alocal = [];

  if ~isempty(varargin{3})
    xlocal = cho_x_local(varargin{3}, param.vars);
  end
  if ~isempty(varargin{4})
    vlocal = cho_x_local(varargin{4}, param.vars);
  end
  if ~isempty(varargin{5})
    alocal = cho_x_local(varargin{5}, param.vars);
  end
  
  qlocal = [xlocal; vlocal];
  Rlocal = zeros(length(param.vars),1);

  if ~isempty(xlocal)
    K = feval(param.wrapmf, 'K', param.rot, param, qlocal, 0);
    if ~isempty(K)
      Rlocal = Rlocal + K * xlocal;
    end
  end
  
  if ~isempty(vlocal)
    D = feval(param.wrapmf, 'D', param.rot, param, qlocal, 0);
    if ~isempty(D)
      Rlocal = Rlocal + D * vlocal;
    end
  end
  
  if ~isempty(alocal)
    M = feval(param.wrapmf, 'M', param.rot, param, qlocal, 0);
    if ~isempty(M)
      Rlocal = Rlocal + M * alocal;
    end
  end

  Flocal = feval(param.wrapmf, 'F', param.rot, param, qlocal, 0);
  if ~isempty(Flocal)
    Rlocal = Rlocal - Flocal;
  end
  
  cho_add_R(assemble_R, param.vars, Rlocal);


 case 'dR'
  
  param       = varargin{1};
  assemble_dR = varargin{2};
  c           = varargin{3};

  xlocal = [];
  vlocal = [];
  alocal = [];

  if ~isempty(varargin{4})
    xlocal = cho_x_local(varargin{4}, param.vars);
  end
  if ~isempty(varargin{5})
    vlocal = cho_x_local(varargin{5}, param.vars);
  end
  if ~isempty(varargin{6})
    alocal = cho_x_local(varargin{6}, param.vars);
  end
  
  qlocal = [xlocal; vlocal];
  dRlocal = zeros(length(param.vars));
  
  if c(1) ~= 0
    K    = feval(param.wrapmf, 'K',    param.rot, param, qlocal, 0);
    dFdx = feval(param.wrapmf, 'dFdx', param.rot, param, qlocal, 0);
    if ~isempty(K)
      dRlocal = dRlocal + c(1) * K;
    end
    if ~isempty(dFdx)
      dRlocal = dRlocal - c(1) * dFdx;
    end
  end
  
  if c(2) ~= 0
    D    = feval(param.wrapmf, 'D',    param.rot, param, qlocal, 0);
    dFdv = feval(param.wrapmf, 'dFdv', param.rot, param, qlocal, 0);
    if ~isempty(D)
      dRlocal = dRlocal + c(2) * D;
    end
    if ~isempty(dFdv)
      dRlocal = dRlocal - c(2) * dFdv;
    end
  end
  
  if c(3) ~= 0
    M = feval(param.wrapmf, 'M', param.rot, param, qlocal, 0);
    if ~isempty(M)
      dRlocal = dRlocal + c(3) * M;
    end
  end
  
  cho_add_dR(assemble_dR, param.vars, dRlocal);
  
  
 case 'output'

  param  = varargin{1};  
  netout = varargin{2};  

  names = fieldnames(param);
  for k = 1:length(names)
    cho_netout(netout, names{k}, getfield(param, names{k}));
  end


 case 'draw'

  % -- CAVEAT: This only has a prayer of working when everything is drawing
  %    into a Matlab window.

  param   = varargin{1};
  netdraw = varargin{2};

  if ~isempty(varargin{3})
    qlocal = cho_x_local(varargin{3}, param.vars);
  else
    qlocal = zeros(length(param.vars), 1);
  end

  feval(param.wrapmf, 'display', param.rot, param, qlocal, 0, param.nodespos);

 
 otherwise
  
  output = [];
  
end
