% function [lat,lon,alt,D,f] = mkchain(N,R,lat_0,lon_0,Zmin,Zmax)

    clc;clear;close all;
    rng(4);
    N = 4;
    R = 150e3;
    lat_0 = 30/180*pi;
    lon_0 = 120/180*pi;
    Zmin = 1000;
    Zmax = 10000;
    
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
            if(sum(d < R & d > 0) == 1 && rho > RE + Zmin && rho < RE + Zmax)
                break;
            else
                continue; 
            end
        end
    end

    % generate trajectory

    X_trj = zeros(5,1);
    Y_trj = zeros(5,1);
    Z_trj = zeros(5,1);

    for i = 1:5
        while true
            if i == 1 || i == 5
                r_trj = 1.5*R;
                pitch_trj = (2*rand(1) - 1) * pi / 2;
                bearing_trj = 2*pi*rand(1);
                if i > N
                    X_trj(i) = X(end) + r_trj * cos(pitch_trj) * cos(pi/2 - bearing_trj);
                    Y_trj(i) = Y(end) + r_trj * cos(pitch_trj) * sin(pi/2 - bearing_trj);
                    Z_trj(i) = Z(end) + r_trj * sin(pitch_trj);                   
                else
                    X_trj(i) = X(i) + r_trj * cos(pitch_trj) * cos(pi/2 - bearing_trj);
                    Y_trj(i) = Y(i) + r_trj * cos(pitch_trj) * sin(pi/2 - bearing_trj);
                    Z_trj(i) = Z(i) + r_trj * sin(pitch_trj);
                end
                dX = X_trj(i) - X(:);
                dY = Y_trj(i) - Y(:);
                dZ = Z_trj(i) - Z(:);
                d =  vecnorm([dX,dY,dZ]',2);
                rho = vecnorm([X_trj(i),Y_trj(i),Z_trj(i)]',2);
                if sum(d < R & d > 0) == 0 && rho > RE + Zmin && rho < RE + Zmax
                    break;
                end
            else
                r_trj = R;
                pitch_trj = (2*rand(1) - 1) * pi / 2;
                bearing_trj = 2*pi*rand(1);
                X_trj(i) = X_trj(i-1) + r_trj * cos(pitch_trj) * cos(pi/2 - bearing_trj);
                Y_trj(i) = Y_trj(i-1) + r_trj * cos(pitch_trj) * sin(pi/2 - bearing_trj);
                Z_trj(i) = Z_trj(i-1) + r_trj * sin(pitch_trj);
                dX = X_trj(i) - X(:);
                dY = Y_trj(i) - Y(:);
                dZ = Z_trj(i) - Z(:);
                d =  vecnorm([dX,dY,dZ]',2);
                rho = vecnorm([X_trj(i),Y_trj(i),Z_trj(i)]',2);
                if sum(d < R & d > 0) == i-1 && rho > RE + Zmin && rho < RE + Zmax
                    break;
                end
            end
        end
        fprintf('finish:%d/%d\n',i,5);
    end
    % X = [X_trj, X];
    % Y = [Y_trj, Y];
    % Z = [Z_trj, Z];

    D = sqrt(((repmat(X,[1,N]) - repmat(X,[1,N])').^2) + ((repmat(Y,[1,N]) - repmat(Y,[1,N])').^2) + ((repmat(Z,[1,N]) - repmat(Z,[1,N])').^2));
    
    D(D==0)=1;
    D(D>R)=0;
    D(D>0)=1;
    D = uint8(D);

    [azm,elev,r] = cart2sph(X,Y,Z);
    [azm_trj,elev_trj,r_trj] = cart2sph(X_trj,Y_trj,Z_trj);

    lon = azm/pi*180;
    lat = elev/pi*180;
    alt = r -RE;

    lon_trj = azm_trj/pi*180;
    lat_trj = elev_trj/pi*180;
    alt_trj = r_trj -RE;

    lon = reshape(lon,size(X));
    lat = reshape(lat,size(X));
    alt = reshape(alt,size(X));

    % lon_trj = reshape(lon_trj,size(X));
    % lat_trj = reshape(lat_trj,size(X));
    % alt_trj = reshape(alt_trj,size(X));

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
    % for i=1:N-1
    %     for j = i+1:N
    %         if D(i,j)==1
    %         line([X(i),X(j)]',[Y(i),Y(j)]','LineStyle',':');
    %         end
    %     end
    % end
    geoplot(gca,lat,lon);
    geoplot(gca,lat_trj,lon_trj);
    hold off;
    HASH = calc_net_hash(lat,lon,alt,D,4,'SHA-1')
    print(f,'-dpng','-r300',['../doc/CHAIN_',num2str(N+1),'_',HASH,'_trj.png']);
    time_step = 165; %s
    save_trj_csv(lat_trj,lon_trj,alt_trj,time_step,['../model/CHAIN_',num2str(N+1),'_',HASH,'.trj']);
    lat = [lat;lat_trj(1)];
    lon = [lon;lon_trj(1)];
    alt = [alt;alt_trj(1)];
    csvwrite(['../model/CHAIN_',num2str(N+1),'_',HASH,'.csv'],[(1:N+1)',lat,lon,alt]);

    % calculate innet time
    syms k;
    tmp = solve(...
    ((1-k)*X_trj(1) + k*X_trj(2) - X(1)) * ((1-k)*X_trj(1) + k*X_trj(2) - X(1)) + ...
    ((1-k)*Y_trj(1) + k*Y_trj(2) - Y(1)) * ((1-k)*Y_trj(1) + k*Y_trj(2) - Y(1)) + ...
    ((1-k)*Z_trj(1) + k*Z_trj(2) - Z(1)) * ((1-k)*Z_trj(1) + k*Z_trj(2) - Z(1)) == ...
    R*R, k);

    time = eval(tmp);

    for i = 1:size(time,1)
        if time(i) < 1
            res = time(i) * time_step
        end
    end

    % L1 = (X_trj(1) - X(1)) * (X_trj(1) - X(1)) + (Y_trj(1) - Y(1)) * (Y_trj(1) - Y(1)) + (Z_trj(1) - Z(1)) * (Z_trj(1) - Z(1))
    % L2 = (X_trj(2) - X(1)) * (X_trj(2) - X(1)) + (Y_trj(2) - Y(1)) * (Y_trj(2) - Y(1)) + (Z_trj(2) - Z(1)) * (Z_trj(2) - Z(1))
    % R2 = R*R


% end