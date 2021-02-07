function [lat,lon,alt,D,f] = mknet(N,R,lat_0,lon_0,Zmin,Zmax)

    % clc;clear;close all;

    % N = 16;
    % R = 150e3;
    % lat_0 = 30/180*pi;
    % lon_0 = 120/180*pi;
    % Zmin = 300;
    % Zmax = 10000;
    
    RE = 6378137.0;
    X = zeros(N,1);
    Y = zeros(N,1);
    Z = zeros(N,1);


    X(:) = RE * cos(lat_0)*cos(lon_0);
    Y(:)  = RE * cos(lat_0)*sin(lon_0);
    Z(:) = RE * sin(lat_0);

    r = ((Zmax - Zmin)*rand(1) + Zmin + RE);
    X(1) = r * cos(lat_0) * cos(lon_0);
    Y(1) = r * cos(lat_0) * sin(lon_0);
    Z(1) = r * sin(lat_0);

    for i = 2:N

        while true
            r = R*(0.2*rand(1)+0.8);
            pitch = (2*rand(1) - 1) * pi / 2;
            bearing = 2*pi*rand(1);
            X(i) = X(i - 1) + r * cos(pitch) * cos(pi/2 - bearing);
            Y(i) = Y(i - 1) + r * cos(pitch) * sin(pi/2 - bearing);
            Z(i) = Z(i - 1) + r * sin(pitch);

            dX = X(i) - X(1:i-1);
            dY = Y(i) - Y(1:i-1);
            dZ = Z(i) - Z(1:i-1);
            d =  vecnorm([dX,dY,dZ]',2);
            rho = vecnorm([X(i),Y(i),Z(i)]',2);
            if i-1 >= N/4
                nb = N/4;
            else
                nb = i-1;
            end
            if(sum(d < R & d > 0) == nb  && rho > RE + Zmin && rho < RE + Zmax)
                break;
            else
                continue; 
            end
        end
    end

    D = sqrt(((repmat(X,[1,N]) - repmat(X,[1,N])').^2) + ((repmat(Y,[1,N]) - repmat(Y,[1,N])').^2) + ((repmat(Z,[1,N]) - repmat(Z,[1,N])').^2));
    
    D(D==0)=1;
    D(D>R)=0;
    D(D>0)=1;
    D = uint8(D);

    [azm,elev,r] = cart2sph(X,Y,Z);

    lon = azm/pi*180;
    lat = elev/pi*180;
    alt = r -RE;

    lon = reshape(lon,size(X));
    lat = reshape(lat,size(X));
    alt = reshape(alt,size(X));

    f = figure;
    f.Units = 'centimeters';
    f.PaperSize=[8,6];
    geoscatter(lat,lon);
    names = cell(N,1);
    for i =1:N
        names{i} = num2str(i);
    end

    text(lat,lon,names,'FontSize',12);

    hold on;
    for i=1:N-1
        for j = i+1:N
            if D(i,j)==1
            line([lat(i),lat(j)]',[lon(i),lon(j)]','LineStyle',':');
            end
        end
    end
    % geoplot(gca,lat,lon);
    hold off;


end