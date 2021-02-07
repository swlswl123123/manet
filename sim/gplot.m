function f1 = gplot(filename)

M = csvread([filename,'.dgraph.csv']);

LOC = csvread([filename,'.node.csv']);


M(M(:,4)==0|M(:,4)==1,:) = [];
M = unique(M,'rows');

e = [M(M(:,3)==2,1),M(M(:,3)==2,2);M(M(:,3)==1,2),M(M(:,3)==1,1)];

e = unique(e,'rows');

G = digraph(e(:,1),e(:,2));

% p = gplot(G);
f1 = figure();
p = plot(G,'Layout','force');
p.XData = LOC(:,2);
p.YData = LOC(:,1);
p.NodeFontSize = 12;
xticks('auto');
yticks('auto');
axis equal;
xlabel('经度/°');
ylabel('纬度/°');

end