% cho_ss_romsos -- 
%   Steady-state analysis by reduced model of second-order system.
% Draws a Bode plot of the reduced system in a specified frequency range.
%
% Inputs:
%   net              -- netlist structure
%   q0               -- linearization point.  [] indicates that SUGAR
%                       should seek an operating point near 0.  A scalar
%                       0 indicates an operating point at 0.
%   P or Pnode, Pvar -- specify the input influence matrix directly or
%                       the node and variable where a signal is applied
%   V or Vnode, Vvar -- specify the output matrix directly or
%                       the node and variable where a signal is observed
%   freqrange        -- Frequency range over which to sample
%                       [freqlo freqhi] gives 1000 samples,
%                       [freqlo freqhi npts] gives npts samples,
%                       [] or no freqrange means no Bode plot is drawn
%   kmax (OPTIONAL)  -- Size of the reduced model / number of Krylov vectors
%                       default value is 10
%   nb   (OPTIONAL)  -- Blocking factor used for block Lanczos
%                       default value is 2
%
% Outputs:
%   Mk,Dk,Kk  -- Mass, damping, and stiffness matrices for reduced model
%   Pk,Vk     -- Reduced input and output matrices
%   L         -- Basis onto which original matrices were projected

function [Mk,Dk,Kk,Pk,Vk,L] = cho_ss_romsos(net, q0, varargin); 


% ---- Get arguments

ndof = cho_mesh_nactive(net);

if isempty(q0)
  q0 = cho_dc(net);
elseif (length(q0) == 1) & (q0 == 0)
  q0 = zeros(ndof,1);
end

argnum = 1;

% -- Get P matrix, or form P given node and variable names
if isnumeric(varargin{argnum})
  P = varargin{argnum};
  argnum = argnum + 1;
else
  nodename = varargin{argnum};
  varname = varargin{argnum + 1};
  P    = zeros(ndof,1);
  Pidx = lookup_coord(net, nodename, varname);
  P(Pidx) = 1;
  argnum = argnum + 2;
end

% -- Get V matrix, or form V given node and variable names
if isnumeric(varargin{argnum})
  V = varargin{argnum};
  argnum = argnum + 1;
else
  nodename = varargin{argnum};
  varname = varargin{argnum + 1};
  V    = zeros(ndof,1);
  Vidx = lookup_coord(net, nodename, varname);
  V(Vidx) = 1;
  argnum = argnum + 2;
end

% -- Get optional frequency range argument
if argnum <= length(varargin)
  freqrange = varargin{argnum};
  if isempty(freqrange)
    npts = 0;
  else
    freqlo = freqrange(1);
    freqhi = freqrange(2);
    if length(freqrange) > 2
      npts = freqrange(3);
    else
      npts = 1000;
    end
  end
  argnum = argnum + 1;
else
  npts = 0;
end

% -- Get optional argument kmax (size of ROM)
if argnum <= length(varargin)
  kmax = varargin{argnum};
  argnum = argnum + 1;
else
  kmax = 10;
end

% -- Get optional argument nb (blocking factor for Lanczos)
if argnum <= length(varargin)
  nb = varargin{argnum};
  argnum = argnum + 1;
else
  nb = 2;
end


% ---- Form system matrices

K = cho_assemble_dR_sparse(net, [1 0 0]);
D = cho_assemble_dR_sparse(net, [0 1 0]);
M = cho_assemble_dR_sparse(net, [0 0 1]);


% ---- Form the reduced-order model

[Mk,Dk,Kk,Pk,Vk,L]=romsos(ndof,nb,kmax,M,D,K,P,V); 


% ---- Draw Bode plot, if one was requested

bode_mdk(npts, freqlo, freqhi, Mk,Dk,Kk,Pk,Vk);

