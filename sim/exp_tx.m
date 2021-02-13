clc;clear;close all;
% nets = {'CHAIN_4_C945','NESTED_16_06BE','STAR_16_8835'};
nets = {'CHAIN_4_B266'};
% nets = {'NESTED_16_06BE'};
% nets = {'STAR_16_8835'};


for i = 1:size(nets,2)

    [f1,f2] = nbplot([nets{i}]);
    
    [f1,f2,f3] = bplot([nets{i}]);
    print(f1,'-dpng','-r300',['../doc/',nets{i},'_TX_B.png']);
    print(f2,'-dpng','-r300',['../doc/',nets{i},'_TX_B_TOTAL.png']);
    print(f3,'-dpng','-r300',['../doc/',nets{i},'_TX_B_MOVE.png']);

    [f1,f2] = qplot([nets{i},'.q.csv']);
    print(f1,'-dpng','-r300',['../doc/',nets{i},'_TX_Q.png']);
    print(f2,'-dpng','-r300',['../doc/',nets{i},'_TX_Q_TOTAL.png']);


    [f1,f2,f3,f4] = dplot(nets{i});
    print(f1,'-dpng','-r300',['../doc/',nets{i},'_DELAY.png']);
    print(f2,'-dpng','-r300',['../doc/',nets{i},'_LOSS.png']);
    print(f3,'-dpng','-r300',['../doc/',nets{i},'_ENDEND.png']);
    print(f4,'-dpng','-r300',['../doc/',nets{i},'_DELAY_PER_HOP.png']);

end