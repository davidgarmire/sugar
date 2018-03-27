relpos_table = {};

-- Add edges from node 1 to 2 and vice-versa
--
function relpos_add(n1, n2, delta)
  local elist;

  elist = relpos_get_elist(n1);
  elist.n = elist.n + 1;
  elist[elist.n] = {  delta[1],  delta[2],  delta[3], n2}

  elist = relpos_get_elist(n2);
  elist.n = elist.n + 1;
  elist[elist.n] = { -delta[1], -delta[2], -delta[3], n1}

  return elist;
end

function relpos_get_elist(n)
  local elist = relpos_table[n];
  if not elist then
    elist = {n = 0};
    relpos_table[n] = elist;
  end
  return elist;
end

function relpos_treewalk()
  local stack = {n = 0};

  for node, elist in relpos_table do
    if node.color == 1 then
      stack.n = stack.n + 1;
      stack[stack.n] = node;
    end
  end

  while stack.n > 0 do
    local node = stack[stack.n]
    stack.n = stack.n - 1

    local elist = relpos_table[node];
    for k = 1, elist.n do
      local e   = elist[k];
      local nbr = e[4]; 
      if nbr.color == 0 then
        nbr[1] = node[1] + e[1];
        nbr[2] = node[2] + e[2];
        nbr[3] = node[3] + e[3];
        node_position(nbr);

        nbr.color = 1;
        stack.n = stack.n + 1;
        stack[stack.n] = nbr;
      end
    end

    node.color = 2
  end

  relpos_assert_all_done();
end

function node(p)
  if p[1] then
    p.color = 1
  else
    p.color = 0
  end
  return %node(p)
end

function relpos_assert_all_done()
  for node, elist in relpos_table do
    if node.color == 0 then
      error("Unpositioned node " .. relpos_node_name(node))
    end
  end
end

function relpos_dump()
  for node, elist in relpos_table do

    if node.color == 0 then
      print("From " .. relpos_node_name(node) .. ":")
    else
      print("From " .. relpos_node_name(node) .. " at ("
            .. node[1] .. ", "
            .. node[2] .. ", "
            .. node[3] .. "):")
    end
    for k = 1, elist.n do
      local e = elist[k];
      print("  to " .. relpos_node_name(e[4]) .. ": (" ..
            e[1] .. ", " ..
            e[2] .. ", " .. 
            e[3] .. ")")
    end

  end
end

function relpos_node_name(n)
  if n.name then
    return n.name .. " (" .. n["mesh index"] .. ")"
  else
    return "(" .. n["mesh index"] .. ")"
  end
end

