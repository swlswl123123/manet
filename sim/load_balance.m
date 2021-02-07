clc;clear;close all;
%%% init net
N = 48;
Qsize = 100;

[~,~,~,~,~,B,X,Y,D] = balloc(48,40,4.5,8,4,4,1);

%%% init traffic

T = 100;    % Packet generation time
P = 1/2;    % Probability of traffic per node over entire time 


% evenly distributed traffic

Traffic = zeros(N,T);
for i = 1:N
    Traffic(i,:) = (rand(1,T)<P).*randi(N,1,T);
end


% init Queues 

Q = zeros(N,Qsize);
Qtop = zeros(N,Qsize);
Qtop(:,1) = ones(N,1);


%%% init beacon

%%%% start loop
for i = 1:T

%%%% apply traffic


Q = Q + repmat(Traffic(:,i),[1,Qsize]).*Qtop;


for j = 1:N
    if(Traffic(j,i)>0)
        Qtop(j,:) = circshift(Qtop(j,:),[0,1]);    
    end
end



return;
%%%% transfer packets


%%%% recv packets

%%%% modify beacons


%%%% judge convergence


end
