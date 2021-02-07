% function [lat,lon,alt,D,f] = mkstar(N,R,lat_0,lon_0,alt_0,Zmin,Zmax)

clc;clear;close all;

rng(4);
N = 16;
R = 150e3;
lat_0 = 30/180*pi;
lon_0 = 120/180*pi;
alt_0 = 20;
Zmin = 1000;
Zmax = 10000;

RE = 6378137.0;

while true
    pitch = rand(N,1) * pi / 6;
    bearing = rand(N,1) * 2 * pi;
    args = zeros(N,N);
    
    X = cos(pitch) .* cos(pi/2 - bearing);
    Y = cos(pitch) .* sin(pi/2 - bearing);
    Z = sin(pitch);

    for i = 1:N
    for j = 1:N
        args(i,j) = acosd((X(i)*X(j) + Y(i)*Y(j) + Z(i)*Z(j)));
    end
    end

    B = sum(sum(args < bw_calc(11.25)));

    if B == N
        break;
    else
        continue;
    end
end
r = 0.8 * rand(N,1) * R + 0.2 * R;


X = r .* cos(pitch) .* cos(pi/2 - bearing);
Y = r .* cos(pitch) .* sin(pi/2 - bearing);
Z = r .* sin(pitch);

X(1) = 0;
Y(1) = 0;
Z(1) = 0;

D = sqrt(((repmat(X,[1,N]) - repmat(X,[1,N])').^2) + ((repmat(Y,[1,N]) - repmat(Y,[1,N])').^2) + ((repmat(Z,[1,N]) - repmat(Z,[1,N])').^2));

D(D==0)=1;
D(D>R)=0;
D(D>0)=1;
D = uint8(D);

[X0,Y0,Z0] = rect2sph1(X,Y,Z,lon_0,lat_0,alt_0);

for i = 1:3
    while true
        if i == 1 || i == 3
            r_trj = 1.5*R;
            pitch_trj = (2*rand(1) - 1) * pi / 2;
            bearing_trj = 2*pi*rand(1);
            X_trj(i) = X0(1) + r_trj * cos(pitch_trj) * cos(pi/2 - bearing_trj);
            Y_trj(i) = Y0(1) + r_trj * cos(pitch_trj) * sin(pi/2 - bearing_trj);
            Z_trj(i) = Z0(1) + r_trj * sin(pitch_trj);
            rho = vecnorm([X_trj(i),Y_trj(i),Z_trj(i)]',2);
            if rho > RE + Zmin && rho < RE + Zmax
                break;
            end
        else
            r_trj = R;
            pitch_trj = (2*rand(1) - 1) * pi / 2;
            bearing_trj = 2*pi*rand(1);
            X_trj(i) = X_trj(i-1) + r_trj * cos(pitch_trj) * cos(pi/2 - bearing_trj);
            Y_trj(i) = Y_trj(i-1) + r_trj * cos(pitch_trj) * sin(pi/2 - bearing_trj);
            Z_trj(i) = Z_trj(i-1) + r_trj * sin(pitch_trj);
            dX = X_trj(i) - X0(1);
            dY = Y_trj(i) - Y0(1);
            dZ = Z_trj(i) - Z0(1);
            d =  vecnorm([dX,dY,dZ]',2);
            rho = vecnorm([X_trj(i),Y_trj(i),Z_trj(i)]',2);
            if d < R && rho > RE + Zmin && rho < RE + Zmax
                break;
            end
        end
    end
    fprintf('finish:%d/%d\n',i,3);
end

[azm,elev,r] = cart2sph(X0,Y0,Z0);
[azm_trj,elev_trj,r_trj] = cart2sph(X_trj,Y_trj,Z_trj);

lon = azm/pi*180;
lat = elev/pi*180;
alt = r -RE;

lon_trj = azm_trj/pi*180;
lat_trj = elev_trj/pi*180;
alt_trj = r_trj -RE;

f = figure;
f.Units = 'centimeters';
f.PaperSize=[8,6];
geoscatter(lat,lon);
geoscatter(lat_trj,lon_trj);
names = cell(N,1);
for i =1:N
names{i} = num2str(i);
end

text(lat,lon,names,'FontSize',12);

hold on;
for i=1
    for j = i+1:N
        if D(i,j)==1
        line([lat(i),lat(j)]',[lon(i),lon(j)]','LineStyle',':');
        end
    end
end
% geoplot(gca,lat,lon);
geoplot(gca,lat_trj,lon_trj);
hold off;

HASH = calc_net_hash(lat,lon,alt,D,4,'SHA-1')
print(f,'-dpng','-r300',['../doc/STAR_',num2str(N+1),'_',HASH,'_trj.png']);
time_step = 165; %s
save_trj_csv(lat_trj',lon_trj',alt_trj',time_step,['../model/STAR_',num2str(N+1),'_',HASH,'.trj']);
lat = [lat';lat_trj(1)];
lon = [lon';lon_trj(1)];
alt = [alt';alt_trj(1)];
csvwrite(['../model/STAR_',num2str(N+1),'_',HASH,'.csv'],[(1:N+1)',lat,lon,alt]);

syms k;
tmp = solve(...
((1-k)*X_trj(1) + k*X_trj(2) - X0(1)) * ((1-k)*X_trj(1) + k*X_trj(2) - X0(1)) + ...
((1-k)*Y_trj(1) + k*Y_trj(2) - Y0(1)) * ((1-k)*Y_trj(1) + k*Y_trj(2) - Y0(1)) + ...
((1-k)*Z_trj(1) + k*Z_trj(2) - Z0(1)) * ((1-k)*Z_trj(1) + k*Z_trj(2) - Z0(1)) == ...
R*R, k);

time = eval(tmp);

for i = 1:size(time,1)
    if time(i) < 1
        res = time(i) * time_step
    end
end



% end