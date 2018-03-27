use("xformstack.lua")

-- Stack of node name prefixes
_namestack = {""; n = 1, nanon = 0};

-- Table of node names in the current scope
_localnames = {};

function subnetize(f)
  return function(p)
    local name = p.name
    if not name then
      name = "anon" .. _namestack.nanon
      _namestack.nanon = _namestack.nanon + 1
    end
    _namestack[_namestack.n + 1] = _namestack[_namestack.n] .. name .. "."
    _namestack.n = _namestack.n + 1
    
    if p.name then
      p.name = _namestack[_namestack.n-1] .. name
    end

    local old_localnames = _localnames
    _localnames = {};

    local T = xform_identity()
    
    if p.oy then
      T = xform_compose(T, xform_oy(p.oy))
      p.oy = nil
    end
    
    if p.oz then
      T = xform_compose(T, xform_oz(p.oz))
      p.oz = nil
    end
    
    if p.ox then
      T = xform_compose(T, xform_ox(p.ox))
      p.ox = nil
    end
    
    xform_push(T)

    %f(p)

    xform_pop()
    _localnames = old_localnames
    _namestack.n = _namestack.n - 1
  end
end

-- Create a new node.  If something with the same name
-- already exists in the current scope, use that.
--
function node(args)
  if type(args) == "string" then
    args = {name = args}
  end

  local name = args.name
  if not name then
    name = "anon" .. _namestack.nanon
    _namestack.nanon = _namestack.nanon + 1
  end

  if not _localnames[name] then 
    args.name = _namestack[_namestack.n] .. name
    _localnames[name] = %node(args)
  end

  return _localnames[name]
end

