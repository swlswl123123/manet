function [lat,lon,alt] = rect2sph(X,Y,Z,lambda,phi,h)

% X = 0;
% Y = 0;
% Z = 0;
% lambda = 120/180*pi;
% phi = 40/180*pi;
% h = 0;

% syms lambda phi RE O gamma alpha h x y g D lat lon alt real
RE = 6378137.0;
R1 = [0,-1,0;1,0,0;0,0,1];
R2 = [sin(phi),0,cos(phi);0,1,0;-cos(phi),0,sin(phi)];
R3 = [cos(lambda),-sin(lambda),0;sin(lambda),cos(lambda),0;0,0,1];

O = R3*R2*R1;


x = [X(:),Y(:),Z(:)]';

g = [(RE+h)*cos(phi)*cos(lambda),(RE+h)*cos(phi)*sin(lambda),(RE+h)*sin(phi)]';
y = O*x + repmat(g,[1,size(X(:),1)]);

[azm,elev,r] = cart2sph(y(1,:),y(2,:),y(3,:));

lon = azm/pi*180;
lat = elev/pi*180;
alt = r -RE;

lon = reshape(lon,size(X));
lat = reshape(lat,size(X));
alt = reshape(alt,size(X));

end