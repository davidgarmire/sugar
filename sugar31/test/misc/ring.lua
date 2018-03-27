function ring(params)

  local n = {}
  local k
  local R
  local theta

  n[1] = params[1];

  R = params.R;
  if R == nil then 
    R = 1;
  end

  x = node_table[n[1]][1] - R;
  y = node_table[n[1]][2];
  z = node_table[n[1]][3];

  for k = 2,params.nring do
    theta = k * (360 / (params.nring + 1));
    n[k] = node{x + R*cos(theta), 
                y + R*sin(theta),
                z}
    beam {n[k-1], n[k] ; layer=params.layer, w=params.w}
  end

  beam {n[1], n[params.nring] ; layer=params.layer, w=params.w}
end

