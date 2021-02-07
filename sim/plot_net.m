function f = plot_net(X,Y,D)
%myFun - Description
%
% Syntax: f = plot_net(X,Y,D)
%
% Long description

N = size(X,1);

f = figure;
f.PaperUnits = 'centimeters';
f.PaperSize=[16,12];
f.Units = 'centimeters';
f.Position = [0,0,16,12];

scatter(X,Y,'filled','r');

names = cell(N,1);
for i =1:N
names{i} = num2str(i);
end

text(X,Y,names,'FontSize',12);

hold on;
for i=1:N-1
    for j = i+1:N
        if D(i,j)==1
        line([X(i),X(j)]',[Y(i),Y(j)]');
        end
    end
end
hold off;

xlabel('x/D_{hop}');
ylabel('y/D_{hop}');
    
end