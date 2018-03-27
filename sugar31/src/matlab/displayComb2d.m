% 2D comb with N fingers displaying;
%     |-----
%     |   -----|
%     |-----   |
%  1  |   -----| 2
% ----+-----   +------     
%     |   -----|
%     |-----   |
%         -----|
% parameter: l = length of the overlap of fingers;
%            L = length of fingers; 
%            permittivity;
%            N  = number of fingers of each set;            
%            h  = thickness (defined in process file);
%            gap = distance between fingers;
%            w = width of fingers;
            
% created by Ningning Zhou  on 08/25/2001;


function displayComb2d(q, pos1, pos2, R, params)

q1=q(1:3);
displayleftComb(q1, pos1, R,params);

rightR = R * [-1,  0, 0;
               0, -1, 0;
               0,  0, 1];

sideLength = 2* (params.w+params.gap);
rightPos = pos2 + R*[0; sideLength/2; 0];
q2=q(5:7);
displayleftComb(q2, rightPos, rightR, params);


function displayleftComb(q, pos1, R, params)

upperN = floor(params.N/2);
firstPos = pos1;
sideLength = 2* (params.w+params.gap);

% draw the left set of fingers;
% draw the straight left finger;
q1([1 2 6 7 8 12]) = [q(1:3),q(1:3)];    % not quite correct;
displaybeam(q1, firstPos, R, params.L,params.w,params.h );

% draw the upper left set of fingers;
for k=1:upperN 
  uLNode(k).pos= firstPos + R*[0; sideLength; 0];
  
  % draw the upper side beam;
  sideR = R * [0, -1, 0;
               1,  0, 0;
               0,  0, 1];
  displaybeam(q1, firstPos, sideR, sideLength, 2*params.w, params.h);

  % draw the upper finger beam;
  displaybeam(q1, uLNode(k).pos, R, params.L,params.w,params.h );  
  firstPos=uLNode(k).pos;
end

% draw the lower left set of fingers;
firstPos = pos1;
for k=1:(params.N-upperN-1) 
  lLNode(k).pos=firstPos + R*[0; -sideLength; 0];

  % draw the side beam
  sideR = R * [0,  1, 0;
              -1,  0, 0;
               0,  0, 1];
  displaybeam(q1, firstPos, sideR, sideLength, 2*params.w, params.h);
  
  % draw the finger beam
  displaybeam(q1, lLNode(k).pos, R, params.L,params.w,params.h );  
  firstPos=lLNode(k).pos;
end

