% Find SS response of linearized system about the equilibrium point q0

function cho_ss(net, q0, in_node, in_var, out_node, out_var)

dof = cho_vars_nactive(net);

% -- Determine influence matrix

B = zeros(2*dof,1);
in_var_id = lookup_coord(net, in_node, in_var);
if (in_var_id ~= 0)
  B(in_var_id) = 1;
end

% -- Determine output matrix

C = zeros(1,2*dof);
out_var_id = lookup_coord(net, out_node, out_var);
if (out_var_id ~= 0)
  C(out_var_id) = 1;
end


% -- Assemble scaled linear system

M = cho_assemble_dR(net, [0 0 1], q0);
D = cho_assemble_dR(net, [0 1 0], q0);
K = cho_assemble_dR(net, [1 0 0], q0);
Id = eye(length(M));
O  = zeros(length(M));

C1 = [D, M; Id,  O];
G1 = [K, O; O, -Id];


% -- Put together state space model (doing straight tf would actually
%    probably be more stable in this case...)

A = C1\G1;
sys = ss(A,B,C,0);


% -- Draw Bode plot

bode(sys);
