function [output] = cho_mesh_get_param(net, material_id, name, mandatory)

if nargin == 3
  mandatory = 1;
end

output = cho_mesh_param(net, name);
if isempty(output) & ~isempty(material_id) 
  output = cho_mesh_material_param(net, material_id, name);
end

if isempty(output)
  error(sprintf('Missing parameter %s', name));
end
