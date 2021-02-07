clc;clear;close all;
rng (12345);

N = 10;
Nb = 40;
T = 20;
HOP = 5;

[X,Y,D,f1] = mknet(N,HOP,1,5);

G = graph(double(D)-eye(N));

figure;
plot(G);


TRAFFIC = zeros(N,N,T);

TRAFFIC(4,2,1)  = 100; %Load 100 4 －> 2 @ T = 1


Q = zeros(N,N);
Bp = zeros(Nb,N,N);
Bn = zeros(Nb,N,N);
FWD = zeros(N,N,N);
BWD = zeros(N,N,N);

% Init beacon

for i = 1:N-1
for j = i+1:N
    if D(i,j) == 1
        Bi = B(:,:,i);
        Bj = B(:,:,j);
        bi = Bi*ones(N,1);
        bj = Bj*ones(N,1);

        vac = find(bi == 0 & bj == 0);
        index = randperm(sum(bi == 0 & bj == 0));

        Bn(vac(index(1)),j,i) = -1;
        Bp(vac(index(2)),j,i) = 1;
        Bp(vac(index(1)),i,j) = 1;
        Bn(vac(index(2)),i,j) = -1;

    end
    next = shortestpath(G,i,j);
    FWD(next(2),j,i) = 1;
    BWD(next(end-1),j,i)

    next = shortestpath(G,j,i);
    FWD(next(2),i,j) = 1;
end
end

Bp = permute(Bp,[2,3,1]);
Bn = permute(Bp,[2,3,1]);


for i = 1:T 

    Q = Q + TRAFFIC(:,:,i);

    for j = 1:Nb
        
    end
end