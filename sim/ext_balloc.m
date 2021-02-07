clc;clear;close all;
rng(0);
n = 16;
r = 150e3;
z_min = 1000;
z_max = 10000;
lat_0 = 30 / 180 * pi;
lon_0 = 120 / 180 * pi;
[lat,lon,alt,W,f] = mkchain(n,r,lat_0,lon_0,z_min,z_max);
W = double(W);

G = digraph(W);

S = zeros(n,n);

for i = 1:n
    for j = 1:n
        if i~=j
            p = shortestpath(G,i,j);
            S(i,j) = p(2);
        end
    end 
end

TFC = randi([0,10],n,n);
TFC = TFC - diag(diag(TFC));

TFCN = zeros(n,n);

for i = 1:n
    for j = 1:n
        if i~= j
            TFCN (i,S(i,j)) = TFCN(i,S(i,j)) + TFC(i,j);
        end
    end
end

i = 4;
N = W(i,:)' | W(:,i);
[D1,D2] = find(N*N'.*TFCN > 0);
D = [D1,D2];
l = size(D,1);
nod = sum(N);
C = zeros(l,1);

while sum(C==0) > 0

    Sm = zeros(l,nod);
    for j = 1 : nod
        for k = 1:l
            if D(k,1) == j
                Sm(k,j) = C(k);
                
            end
        end
    end
end








