function [id] = lookup_coord(net, nodename, coord)

node_id = cho_mesh_node_lookup(net, nodename);
if isempty(node_id)
  id = [];
else
  id = cho_vars_lookup_node(net, node_id, coord);
  if (id == 0)
    id = [];
  end
end

