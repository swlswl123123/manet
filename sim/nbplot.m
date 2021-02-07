function [f1,f2] = nbplot(netname)

F = csvread([netname,'.nb.csv']);

l = size(F,1);

T  = F(:,1);
ID = int64(F(:,2));
NB = int64(F(:,3));


UID = unique(ID);
UT = unique(T);

n = size(UID,1);
t = size(UT,1);

N = zeros(n,1);

for i = 1:n
    N(i) = sum(ID==UID(i));
end

Y = zeros(t,n);


for i = 1:l

    r = find(UT==T(i));
    c = find(UID==ID(i));
    Y(r,c) =  NB(i);
end

% for i = 1:t
%     for j = 1:n
%     Y(i,j) = sum(NB(T==UT(i)&ID==UID(j)));
%     end
% end

f1 = figure;

plot(UT,Y);
xlabel('仿真时间/秒');
ylabel('可用路由数');

% hold on;
% for i = 1:n
%     text(UT(end),Y(end,i),num2str(UID(i)));
% end
% hold off;

f2 = figure;

AY = zeros(t,1);

for i = 1:t
AY(i) = sum(NB(T==(UT(i))))/(n*(n-1))*100;
end

plot(UT,AY);
xlabel('仿真时间/秒');
ylabel('路由发现率/%');

end