clc;clear;close all;
% nets = {'16_8_F451','48_8_CEE7','20_1_8010'};
% nets = {'48_8_CEE7'};
nets = {'16_grid'};
% nets = {'STAR_17_5203'};
% nets = {'STAR_16_8835'};
% nets = {'NESTED_16_06BE'};
% nets = {'CHAIN_4_C945','NESTED_16_DEF3'};
% nets = {'CHAIN_4_C945'};

% nets = {'STAR_17_5203'};
% nets = {'NESTED_16_DEF3'};

for i = 1:size(nets,2)

    [f1,f2] = nbplot(nets{i});
    print(f1,'-dpng','-r300',['../doc/',nets{i},'_ND_NB.png']);
    print(f2,'-dpng','-r300',['../doc/',nets{i},'_ND_NB_TOTAL.png']);

    [f1,f2] = bplot(nets{i});
    print(f1,'-dpng','-r300',['../doc/',nets{i},'_ND_B.png']);
    print(f2,'-dpng','-r300',['../doc/',nets{i},'_ND_B_TOTAL.png']);

    % f1 = gplot(nets{i});
    % print(f1,'-dpng','-r300',[nets{i},'_ND_topo_recon.png']);

    % [f1,f2] = qplot([nets{i},'.q.csv']);
    % print(f1,'-dpng','-r300',[nets{i},'_ND_Q.png']);
    % print(f2,'-dpng','-r300',[nets{i},'_ND_Q_TOTAL.png']);

end