%perforated.m for 2 or more holes.
%This subnet makes a perforated beam. 
%Jason Vaughn Clark - Nov2001 

subnet perforated [left right][numberholes=* w=* h=* l=* whorizontal=* wvertical=*] 
[
   LH=(l-wvertical)/numberholes-wvertical
   LV=w-whorizontal*2
   %Make n-1 inner holes
   for i=1:numberholes-1
   [
      beam3dlinkcorner parent [a(i) a(i+1)][l=LH w=whorizontal h=h L1=wvertical/2 L2=wvertical/2] %Horizontal.
      beam3dlinkcorner parent [b(i) b(i+1)][l=LH w=whorizontal h=h L1=wvertical/2 L2=wvertical/2] %Horizontal.
      beam3dlink parent [a(i+1) b(i+1)][l=LV w=wvertical h=h L1=whorizontal/2 L2=whorizontal/2 oz=-pi/2] %Vertical.
   ]
   %Make left end
   beam3dlink parent [a(1) left][l=LV/2 w=wvertical h=h L1=whorizontal/2 L2=wvertical/2 oz2=-pi/2 oz=-pi/2] %Vertical.
   beam3dlink parent [left b(1)][l=LV/2 w=wvertical h=h L1=wvertical/2 oz1=pi/2 L2=whorizontal/2 oz=-pi/2] %Vertical.
   %Make right end
   beam3dlinkcorner parent [a(numberholes) a(numberholes+1)][l=LH w=whorizontal h=h L1=wvertical/2 L2=wvertical/2] %Horizontal.
   beam3dlinkcorner parent [b(numberholes) b(numberholes+1)][l=LH w=whorizontal h=h L1=wvertical/2 L2=wvertical/2] %Horizontal.
   beam3dlink parent [a(numberholes+1) right][l=LV/2 w=wvertical h=h L1=whorizontal/2 L2=wvertical/2 oz2=pi/2 oz=-pi/2] %Vertical.   
   beam3dlink parent [right b(numberholes+1)][l=LV/2 w=wvertical h=h L1=wvertical/2 oz1=-pi/2 L2=whorizontal/2 oz=-pi/2] %Vertical.   
]


