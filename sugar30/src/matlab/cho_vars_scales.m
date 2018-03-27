function [scales] = cho_vars_scales(net)

nactive = cho_vars_nactive(net);
vars_info = cho_vars_lookup(net);

for i = 1:nactive
  scales(i,1) = default_scales(vars_info(i).name);
end

