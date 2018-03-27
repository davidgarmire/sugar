xform_stack = {n = 0};

function xform_push(T)
  local n = xform_stack.n + 1
  if n == 1 then
    xform_stack[n] = T
  else
    xform_stack[n] = xform_compose(xform_stack[n-1], T);
  end
  xform_stack.n = n;
end

function xform_pop()
  xform_stack.n = xform_stack.n - 1
end

function top_xform(x)
  local n = xform_stack.n
  if n > 0 then
    return xform_apply(xform_stack[n], x)
  else
    return x
  end
end 

-- Overwrite x with Tx
function xform_apply(T, x)
  local y1 = T[1]*x[1] + T[4]*x[2] + T[7]*x[3] + T[10];
  local y2 = T[2]*x[1] + T[5]*x[2] + T[8]*x[3] + T[11];
  local y3 = T[3]*x[1] + T[6]*x[2] + T[9]*x[3] + T[12];
  x[1] = y1;
  x[2] = y2;
  x[3] = y3;
  return x;
end

-- Overwrite x with A_T * x
function xform_applyA(T, x)
  local y1 = T[1]*x[1] + T[4]*x[2] + T[7]*x[3]; 
  local y2 = T[2]*x[1] + T[5]*x[2] + T[8]*x[3]; 
  local y3 = T[3]*x[1] + T[6]*x[2] + T[9]*x[3];
  x[1] = y1;
  x[2] = y2;
  x[3] = y3;
  return x;
end

-- Return TS
function xform_compose(T, S)
  local TS = {};
  for k = 1,4 do
    local base = 3*k-3;
    TS[base+1] = T[1]*S[base+1] + T[4]*S[base+2] + T[7]*S[base+3];
    TS[base+2] = T[2]*S[base+1] + T[5]*S[base+2] + T[8]*S[base+3];
    TS[base+3] = T[3]*S[base+1] + T[6]*S[base+2] + T[9]*S[base+3];
  end
  TS[10] = TS[10] + T[10];
  TS[11] = TS[11] + T[11];
  TS[12] = TS[12] + T[12];
  return TS;
end

-- Return the identity
function xform_identity()
  return { 1, 0, 0,
           0, 1, 0,
           0, 0, 1,
           0, 0, 0 };
end

-- Return a rotation about the x axis
function xform_ox(r)
  local c = cos(r)
  local s = sin(r)
  return { 1,   0,  0,
           0,   c,  s,
           0,  -s,  c,
           0,   0,  0 }
end

-- Return a rotation about the y axis
function xform_oy(r)
  local c = cos(r)
  local s = sin(r)
  return { c,   0, -s,
           0,   1,  0,
           s,   0,  c,
           0,   0,  0 }
end

-- Return a rotation about the z axis
function xform_oz(r)
  local c = cos(r)
  local s = sin(r)
  return { c,   s,  0,
          -s,   c,  0,
           0,   0,  1,
           0,   0,  0 }
end

-- Return a translation 
function xform_translate(z)
  return { 1,    0,    0,
           0,    1,    0,
           0,    0,    1,
           z[1], z[2], z[3] }
end

function nodex(p)
  if p[1] then
    top_xform(p)
  end
  return node(p)
end

