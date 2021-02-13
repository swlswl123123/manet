function [f1,f2,f3] = bplot(netname)

F = csvread([netname,'.b.csv']);

l = size(F,1);

T  = F(:,1);
ID = int64(F(:,2));
B = int64(F(:,3));


UID = unique(ID);
UT = unique(T);
UID = sort(UID);

n = size(UID,1);
t = size(UT,1);

N = zeros(n,1);

for i = 1:n
    N(i) = sum(ID==UID(i));
end

Y = zeros(t,n);
Y2 = zeros(t);

for i = 1:l

    r = find(UT==T(i));
    c = find(UID==ID(i));
    Y(r,c) =  B(i);
    Y2(r) = Y2(r)+B(i);
end

Y = Y/49*100;
Y2 = Y2/49/n*100;

f1=figure;

plot(UT,Y);

% hold on;
% for i = 1:n
%     text(UT(end),Y(end,i),num2str(UID(i)));
% end
% hold off;
% end

ylabel('节点时隙利用率/%');
xlabel('仿真时间/秒');

f2 = figure;

plot(UT,Y2);
ylabel('全网时隙利用率/%');
xlabel('仿真时间/秒');

f3 = figure;

plot(UT,Y(:,end))
ylabel('移动节点时隙利用率/%');
xlabel('仿真时间/秒');

