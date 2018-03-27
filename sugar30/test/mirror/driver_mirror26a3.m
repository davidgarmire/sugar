net = cho_load('mirror26a3.net');

ndof = cho_mesh_nactive(net);
P = zeros(ndof,1);
P(2) = 20;
V = zeros(ndof,1);
V(22) = 1;

cho_ss_romsos(net, 0, P, V, [0 10000]);
