function [value] = dqval(net,vector,nodename,coordinate)

[index] = lookup_coord(net, nodename, coordinate);
value = vector(index);