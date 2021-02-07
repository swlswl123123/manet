clc;clear;close all;
rng(10); %Set random seed to maintain the reproducibility of the simulation result

N = 16;
hop = 8;
W = 8;
H = 1;
R = 150e3;

[X,Y,D] = mkgrid(4,4);    % Generate random network

[lat,lon,alt] = rect2sph(R*X,R*Y,zeros(size(X)),125/180*pi,31/180*pi,5000);

% alt(:)=5000;

f = figure;
f.Units = 'centimeters';
% f.PaperSize=[8,6];
geoscatter(lat,lon);
names = cell(N,1);
for i =1:N
    names{i} = num2str(i);
end

text(lat,lon,names,'FontSize',12);

hold on;
for i=1:N-1
    for j = i+1:N
        if D(i,j)==1
        line([lat(i),lat(j)]',[lon(i),lon(j)]','LineStyle',':');
        end
    end
end

save_net_csv(lon,lat,alt,[num2str(N),'_grid.csv']);
print(f,'-dpng','-r300',[num2str(N),'_grid.png']);