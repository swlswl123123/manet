clc;clear;close all;
nets = {'SYNC_16_8_F451','SYNC_48_8_CEE7','SYNC_20_1_8010'};
% nets = {'SYNC_48_8_CEE7'};
% nets = {'SYNC_20_1_8010'};
% nets = {'SYNC_16_8_F451'};
% nets = {'48_grid'};
% nets = {'724network2'};

for i = 1:size(nets,2)
    
    [f1,f2,f3] = splot(nets{i});
    print(f1,'-dpng','-r300',['../doc/',nets{i},'_SYNC_RATE.png']);
    print(f2,'-dpng','-r300',['../doc/',nets{i},'_CLOCK.png']);
    print(f3,'-dpng','-r300',['../doc/',nets{i},'_CLOCK_ERROR.png']);

end

