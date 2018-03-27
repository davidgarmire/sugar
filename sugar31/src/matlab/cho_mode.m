% Find the resonating frequencies and corresponding mode shapes
% (eigenvalues and eigenvectors) for the linearized system about
% an equilibrium point.
%
% Inputs:
%   net     - netlist structure
%   nmodes  - (Optional)  If > 0, use sparse solvers to get nmodes modes
%             If == 0, use the usual dense solver to get all the modes
%             If < 0, solve with eig(K\M) rather than eig(M,K).  This last
%             option can potentially cause trouble, but it is faster.
%   q0      - (Optional) equilibrium operating point.  If not supplied,
%             the routine will search for an equilibrium point near 0.
%   find_dc - (Optional)  If true, search for an equilibrium point
%             near the supplied q0
%              
% Outputs:
%   freq - vector of resonating frequencies (eigenvalues)
%   egv  - array of corresponding mode shapes (eigenvectors)
%   q0   - equilibrium point

function [freq, egv, q0] = cho_mode(net, nmodes, q0, find_dc);


% -- Find equilibrium point

if (nargin <= 2)
  q0 = cho_dc(net);
elseif (nargin >= 4)
  if (find_dc == 1)
    q0 = cho_dc(net, q0);
  end
elseif (isempty(q0))
  q0 = zeros(cho_mesh_nactive(net), 1);
end

if (nargin < 2)
  nmodes = 0;
end

is_sp = (nmodes > 0);


% -- Get the M and K-dF matrices out (we ignore damping for the moment)

if is_sp
  M = cho_assemble_dR_sparse(net, [0 0 1]);
  K = cho_assemble_dR_sparse(net, [1 0 0]);
else
  M = cho_assemble_dR(net, [0 0 1]);
  K = cho_assemble_dR(net, [1 0 0]);
end


% -- Do the eigendecomposition, etc.  This is mostly copied out of the
%    old code and made slightly more efficient.

if nmodes > 0

  % Note: Ideally, we should do a generalized eigenproblem solve.
  %  Unfortunately, eigs goes ballistic when I try that.  Perhaps
  %  once we have the scaling right...
  % This code will almost certainly break when K is singular
  %  (as occurs with some electrical elements).

  M = (M+M')/2;  
  K = (K+K')/2;  % This may be bogus if we really do have asymmetry
  options.issym = 1;  % It is symmetric
  options.disp = 0;   % Don't display intermediate steps
  [V, f1_inv, flag] = eigsdb(M, K, nmodes, 'LR', options);
  if flag ~= 0
    disp('Warning: eigs may have failed to converge');
    flag
  end

elseif nmodes == 0

  % This may well be bogus if we lose symmetry.  That seems to happen
  % for the gap model currently, but I think there may be a problem
  % in the Jacobian computation there.

  [V, f1_inv] = eig(M,K);  % This is a lot slower, if perhaps more stable

else

  [V, f1_inv] = eig(K\M);

end

f1_inv = diag(f1_inv);

nz = find( (abs(f1_inv) > 1e-40) & (abs(f1_inv) < inf) );
freq = sqrt(1./f1_inv(nz));

[freq, P] = sort(freq);
egv = V(:,nz(P));
