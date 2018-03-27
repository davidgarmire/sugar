% bode_mdk --
% Draw Bode plots for a second order SISO system
%   Ma + Dv + Kx = Pu
%   y = Vx
%
% Inputs:
%   npts -- Number of points
%   freqlo, freqhi -- frequency range to be plotted
%   M, D, K, P, V  -- system matrices

function bode_mdk(npts, freqlo, freqhi, Mk, Dk, Kk, Pk, Vk)

freq = linspace(freqlo,freqhi,npts);
s    = 2*pi*sqrt(-1)*freq;

for i = 1:npts,
   Hk(i) = Vk'*((s(i)^2*Mk + s(i)*Dk + Kk)\Pk);
end

figure(1)
subplot(2,1,1)
plot(freq,20*log10(abs(Hk)),'b--');
xlabel('Frequency (Hz)')
ylabel('Gain (dB)')
title('Bode plot')

subplot(2,1,2)
plot(freq,angle(Hk)*180/pi,'b--');
xlabel('Frequency (Hz)')
ylabel('phase(degree)')

