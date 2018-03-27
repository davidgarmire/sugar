% Display the shape of a resonating mode of the mechanical structure.
%
% Inputs:
%   net  - netlist structure
%   freq - vector of resonant frequencies from cho_mode
%   egv  - array of mode shape vectors from cho_mode
%   q0   - equilibrium point from cho_mode
%   s    - scale factor (optional)
%   num  - number of the mode to display

function cho_modeshape(net, freq, egv, q0, s, num);


% -- Set default scale factor if not specified
%    Ideally, this would involve some query to the model functions or
%    at least some slightly less ad-hoc way of determining things

if nargin == 5
   num = s;   
   dq0 = abs(real(egv( 1:length(q0), num )));
   dq0(find(dq0 < eps)) = eps;
   scales = cho_vars_scales(net);
   dq0rel = scales ./ dq0;
   s = 120*min(dq0rel);
end


% -- Get the displacement vector and frequency value for display
%    Note that we need to throw away the velocity half of the
%    mode shape stuff...

dq0 = real(egv( 1:length(q0), num ));
q = q0 + s*dq0;
f = freq(num) / 2 / pi;


% -- Display the displaced structure and give a nice title
cho_display(net, q);
title(sprintf('Mode %d, frequency %f Hz', num, f));
