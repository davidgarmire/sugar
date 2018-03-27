function cho_display(mesh, x, gctype)

if nargin < 3
  gctype = 'matlab';
end

if strcmp(gctype, 'xt')
  if (nargin > 1) & ~isempty(x)
    sugarmex('xt_draw', mesh, x)    
  else
    sugarmex('xt_draw', mesh)
  end
elseif strcmp(gctype, 'matlab')
  if (nargin > 1) & ~isempty(x)
    cho_mdraw(mesh, x)
  else
    cho_mdraw(mesh)
  end
end
