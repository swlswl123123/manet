clc;clear;close all;
rng(0);
delete('*.tx.csv')

% nets = {'CHAIN_4_C945','NESTED_16_DEF3','STAR_17_5203','CHAIN_5_63CF','NESTED_17_DEF3','STAR_17_7B47'};
N = [4,16,16];
R = [2400,1200,588];
S = [12000,12000,12000];
% Ts = [10,10,10,330,340,100];
% Te = [20,15,20,340,345,110];
% centre_id = [-1,-1,1,5,17,1];
% nets = {'CHAIN_4_C563'};
nets = {'CHAIN_4_C945','NESTED_16_06BE','STAR_16_8835'};
centre_id = [-1,-1,1];
Ts = [10,10,10];
Te = [12,12,11.5];

% N = 4;
% R = 1;
% S = 12000;
% Ts = 50;
% Te = 55;


for i = 1:size(nets,2)

    f2 = gentraffic_even(N(i),Ts(i),Te(i),S(i),R(i),centre_id(i),[nets{i},'.tx.csv']);

    % print(f1,'-dpng','-r300',[nets{i},'_TRAFFIC_TX_DIST.png']);
    % gentraffic_new(Ts(i), Te(i), nets{i});
    % print(f2,'-dpng','-r300',['../doc/',nets{i},'_TRAFFIC_TX_DIST_TIME.png']);
end

% nets = {'STAR_17_5203'};
% N = 17;
% R = 1000;
% S = 12000;
% Ts = 30;
% Te = 40;

% for i = 1:size(nets,2)

%     f2 = gentraffic_even(N(i),Ts(i),Te(i),S(i),R(i),['../model/',nets{i},'.tx.csv']);

%     % print(f1,'-dpng','-r300',['/home/victor/Projects/54-doc/image/',nets{i},'_TRAFFIC_TX_DIST.png']);
%     print(f2,'-dpng','-r300',['/home/victor/Projects/54-doc/image/',nets{i},'_TRAFFIC_TX_DIST_TIME.png']);

% end