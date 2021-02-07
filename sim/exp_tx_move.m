clc;clear;close all;
%nets = {'16_8_F451','48_8_CEE7','20_1_8010'};
% nets = {'CHAIN_4_C945'};
% nets = {'CHAIN_5_63CF'};
nets = {'STAR_17_7B47'};
% nets = {'48_8_CEE7'};
% nets = {'16_8_F451'};
% nets = {'48_grid'};
% nets = {'NESTED_16_DEF3'};
% nets = {'NESTED_17_DEF3'};

for i = 1:size(nets,2)
    
    [f1,f2,f3] = bplot([nets{i}]);
    print(f1,'-dpng','-r300',['../doc/',nets{i},'_TX_B.png']);
    print(f2,'-dpng','-r300',['../doc/',nets{i},'_TX_B_TOTAL.png']);
    print(f3,'-dpng','-r300',['../doc/',nets{i},'_TX_B_MOVE.png']);

    % [f1,f2] = qplot([nets{i},'.q.csv']);
    % print(f1,'-dpng','-r300',['../doc/',nets{i},'_TX_Q.png']);
    % print(f2,'-dpng','-r300',['../doc/',nets{i},'_TX_Q_TOTAL.png']);


    [f1,f2,f3,f4,f5,f6] = dplot_move(nets{i});
    print(f1,'-dpng','-r300',['../doc/',nets{i},'_DELAY.png']);
    print(f2,'-dpng','-r300',['../doc/',nets{i},'_LOSS.png']);
    print(f3,'-dpng','-r300',['../doc/',nets{i},'_ENDEND.png']);
    print(f4,'-dpng','-r300',['../doc/',nets{i},'_THROUGHPUT.png']);
    print(f5,'-dpng','-r300',['../doc/',nets{i},'_DELAY_PER_HOP.png']);
    print(f6,'-dpng','-r300',['../doc/',nets{i},'.png']);
end