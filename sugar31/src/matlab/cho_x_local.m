function [xlocal] = cho_x_local(assembler, var_ids)
xlocal = sugarmex('x_local', assembler, var_ids);
