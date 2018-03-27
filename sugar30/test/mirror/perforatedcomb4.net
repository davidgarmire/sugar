%perforatedcomb4.m 
%This subnet makes a perforated beam. 
%Jason Vaughn Clark - Nov2001 

subnet perforatedcomb4 [left right][numberfingers=* w=* h=* whorizontal=* wfinger=* lfinger=* gap=* wvertical=* L1=* L2=*] 
[
   numberholes=numberfingers+1
   l=((gap*2+wfinger)+wfinger)*numberholes+wfinger
   LH=(l-wvertical)/numberholes-wvertical
   LV=w-whorizontal*2
   %Make n-1 inner holes
   for i=1:numberholes-1
   [
      beam3dlinkcorner parent [a(i) a(i+1)][l=LH w=whorizontal h=h L1=wvertical/2 L2=wvertical/2] %Horizontal.
      beam3dlinkcorner parent [b(i) b(i+1)][l=LH w=whorizontal h=h L1=wvertical/2 L2=wvertical/2] %Horizontal.
      beam3dlink parent [a(i+1) b(i+1)][l=LV w=wvertical h=h L1=whorizontal/2 L2=whorizontal/2 oz=-pi/2] %Vertical.
      beam3dlink parent [b(i+1) c(i+1)][l=lfinger w=wfinger h=h L1=whorizontal/2 oz=-pi/2] %Finger.      
   ]
   %Make left end
   beam3dlink parent [a(1) left][l=LV/2 w=wvertical h=h L1=whorizontal/2 L2=wvertical/2+L1 oz2=-pi/2 oz=-pi/2] %Vertical.
   beam3dlink parent [left b(1)][l=LV/2 w=wvertical h=h L1=wvertical/2+L1 oz1=pi/2 L2=whorizontal/2 oz=-pi/2] %Vertical.
   beam3dlink parent [b(1) c(1)][l=lfinger w=wfinger h=h L1=whorizontal/2 oz=-pi/2] %Finger.         
   %Make right end
   beam3dlinkcorner parent [a(numberholes) a(numberholes+1)][l=LH w=whorizontal h=h L1=wvertical/2 L2=wvertical/2] %Horizontal.
   beam3dlinkcorner parent [b(numberholes) b(numberholes+1)][l=LH w=whorizontal h=h L1=wvertical/2 L2=wvertical/2] %Horizontal.
   beam3dlink parent [a(numberholes+1) right][l=LV/2 w=wvertical h=h L1=whorizontal/2 L2=L2+wvertical/2 oz2=pi/2 oz=-pi/2] %Vertical.   
   beam3dlink parent [right b(numberholes+1)][l=LV/2 w=wvertical h=h L1=L2+wvertical/2 oz1=-pi/2 L2=whorizontal/2 oz=-pi/2] %Vertical.   
   beam3dlink parent [b(numberholes+1) c(numberholes+1)][l=lfinger w=wfinger h=h L1=whorizontal/2 oz=-pi/2] %Finger.         
]


