function cho_add_displace(assembler, var_id, value)

if nargin == 2
    value = 0;
end
sugarmex('add_displace', assembler, var_id, value);
