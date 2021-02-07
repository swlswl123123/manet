clc;clear;

G = [1,0,0;0,1,0;0,0,1];

syms lambda phi RE O gamma alpha h x y g D lat lon alt real

R1 = [0,-1,0;1,0,0;0,0,1];
R2 = [sin(phi),0,cos(phi);0,1,0;-cos(phi),0,sin(phi)];
R3 = [cos(lambda),-sin(lambda),0;sin(lambda),cos(lambda),0;0,0,1];

O = R3*R2*R1;


x = [D*cos(alpha)*sin(gamma),D*cos(alpha)*cos(gamma),D*sin(alpha)]';
g = [(RE+h)*cos(phi)*cos(lambda),(RE+h)*cos(phi)*sin(lambda),(RE+h)*sin(phi)]';
y = O*x + g;



RE = 6378137.0;
D = 100000;
lambda = 0;
phi = 0;
h = 0;
gamma = pi/2;
alpha =   0;




% S = solve(subs(y == [(RE+alt)*cos(lat)*cos(lon),(RE+alt)*cos(lat)*sin(lon),(RE+alt)*sin(lat)]'),lat,lon,alt);

