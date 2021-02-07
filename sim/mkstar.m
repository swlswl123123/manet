function [lat,lon,alt,D,f] = mkstar(N,R,lat_0,lon_0,alt_0,Zmin,Zmax)

    % clc;clear;close all;

    % N = 16;
    % R = 150e3;
    % lat_0 = 30/180*pi;
    % lon_0 = 120/180*pi;
    % Zmin = 300;
    % Zmax = 10000;
    
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

    [lat,lon,alt] = rect2sph(X,Y,Z,lon_0,lat_0,alt_0);

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