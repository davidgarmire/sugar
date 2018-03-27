function [Qd,Qv]=dkmr(n,nb,k,M,D,K,P,V)

%
% initialization 
%

KR = chol(K);

R0d = KR\(KR'\[P V]); 
R0v = zeros(n,nb); 
Temp = R0d'*K*R0d; 
[Ur,Sr,Vr]=svd(Temp);  

Qd(:,1:nb) = R0d*(Ur/sqrt(Sr));
Qv(:,1:nb) = zeros(n,nb);

%
% Main loop 
%
for j = 1:k-1,  

    Rd(:,(j-1)*nb+1:j*nb) = KR\(KR'\( (D*Qd(:,(j-1)*nb+1:j*nb)) + ...
			              (M*Qv(:,(j-1)*nb+1:j*nb)) )); 
    Rv(:,(j-1)*nb+1:j*nb) = -Qd(:,(j-1)*nb+1:j*nb); 
%
%   orthogonalization
%
    for i = 1:j,  

        Ai = Qd(:,(i-1)*nb+1:i*nb)'*K*Rd(:,(j-1)*nb+1:j*nb);

        Rd(:,(j-1)*nb+1:j*nb) = Rd(:,(j-1)*nb+1:j*nb) - ...
		                       Qd(:,(i-1)*nb+1:i*nb)*Ai; 

        Rv(:,(j-1)*nb+1:j*nb) = Rv(:,(j-1)*nb+1:j*nb) - ...
		                       Qv(:,(i-1)*nb+1:i*nb)*Ai; 
    end 

%   normalization

    Temp = Rd(:,(j-1)*nb+1:j*nb)'*K*Rd(:,(j-1)*nb+1:j*nb);  
    [Ur,Sr,Vr] = svd(Temp); 
    Qd(:,j*nb+1:(j+1)*nb) = Rd(:,(j-1)*nb+1:j*nb)*(Ur/sqrt(Sr));  
    Qv(:,j*nb+1:(j+1)*nb) = Rv(:,(j-1)*nb+1:j*nb)*(Ur/sqrt(Sr));  

end 
