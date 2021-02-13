clc;clear;close all;
rng(2); %Set random seed to maintain the reproducibility of the simulation result
RE = 6378137.0;

N = 4;
R = 30e3;
Zmin = 200;
Zmax = 2000;

[lat,lon,alt,D,f] = mkchain(N,R,30/180*pi,120/180*pi,Zmin,Zmax);

HASH = calc_net_hash(lat,lon,alt,D,4,'SHA-1');    % Calculate the unique identifier of the network
D = double(D) - eye(size(D));
[p,q,v] = find(D);
csvwrite(['CHAIN_',num2str(N),'_',HASH,'.graph.csv'],[p,q,v]);
csvwrite(['CHAIN_',num2str(N),'_',HASH,'.node.csv'],[lat,lon,alt]);

csvwrite(['../model/CHAIN_',num2str(N),'_',HASH,'.csv'],[(1:N)',lat,lon,alt]);

% print(f,'-dpng','-r300', ['/home/victor/Projects/54-doc/image/CHAIN_',num2str(N),'_',HASH,'_GEO.png']);

disp(HASH);


% clear;close all;
% rng(4); %Set random seed to maintain the reproducibility of the simulation result
% RE = 6378137.0;

% N = 16;
% R = 150e3;
% Zmin = 1000;
% Zmax = 10000;

% [lat,lon,alt,D,f] = mkchain(N,R,30/180*pi,120/180*pi,Zmin,Zmax);

% HASH = calc_net_hash(lat,lon,alt,D,4,'SHA-1');    % Calculate the unique identifier of the network
% D = double(D) - eye(size(D));
% [p,q,v] = find(D);
% csvwrite(['CHAIN_',num2str(N),'_',HASH,'.graph.csv'],[p,q,v]);
% csvwrite(['CHAIN_',num2str(N),'_',HASH,'.node.csv'],[lat,lon,alt]);

% csvwrite(['../model/CHAIN_',num2str(N),'_',HASH,'.csv'],[(1:N)',lat,lon,alt]);

% print(f,'-dpng','-r300', ['/home/victor/Projects/54-doc/image/CHAIN_',num2str(N),'_',HASH,'_GEO.png']);

% disp(HASH);

% clear;close all;
% rng(4); %Set random seed to maintain the reproducibility of the simulation result
% RE = 6378137.0;

% N = 16;
% R = 150e3;
% Zmin = 1000;
% Zmax = 10000;

% [lat,lon,alt,D,f] = mknet(N,R,30/180*pi,120/180*pi,Zmin,Zmax);

% HASH = calc_net_hash(lat,lon,alt,D,4,'SHA-1');    % Calculate the unique identifier of the network
% D = double(D) - eye(size(D));
% [p,q,v] = find(D);
% csvwrite(['NESTED_',num2str(N),'_',HASH,'.graph.csv'],[p,q,v]);
% csvwrite(['NESTED_',num2str(N),'_',HASH,'.node.csv'],[lat,lon,alt]);

% csvwrite(['../model/NESTED_',num2str(N),'_',HASH,'.csv'],[(1:N)',lat,lon,alt]);

% print(f,'-dpng','-r300', ['/home/victor/Projects/54-doc/image/NESTED_',num2str(N),'_',HASH,'_GEO.png']);

% disp(HASH);

% clear;close all;
% rng(4); %Set random seed to maintain the reproducibility of the simulation result
% RE = 6378137.0;

% N = 17;
% R = 150e3;
% Zmin = 1000;
% Zmax = 10000;

% [lat,lon,alt,D,f] = mkstar(N,R,30/180*pi,120/180*pi,20,Zmin,Zmax);

% HASH = calc_net_hash(lat,lon,alt,D,4,'SHA-1');    % Calculate the unique identifier of the network
% D = double(D) - eye(size(D));
% [p,q,v] = find(D);
% csvwrite(['STAR_',num2str(N),'_',HASH,'.graph.csv'],[p,q,v]);
% csvwrite(['STAR_',num2str(N),'_',HASH,'.node.csv'],[lat,lon,alt]);

% csvwrite(['../model/STAR_',num2str(N),'_',HASH,'.csv'],[(1:N)',lat,lon,alt]);

% print(f,'-dpng','-r300', ['/home/victor/Projects/54-doc/image/STAR_',num2str(N),'_',HASH,'_GEO.png']);

% disp(HASH);