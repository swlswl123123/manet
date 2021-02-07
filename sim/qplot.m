function [f1,f2] = qplot(filename)

F = csvread(filename);

l = size(F,1);

T  = F(:,1);
ID = int64(F(:,2));
Q = int64(F(:,3));
S = int64(F(:,4));


UID = unique(ID);
UT = unique(T);

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
    Y(r,c) =  Y(r,c) + S(i);
    Y2(r) = Y2(r) + S(i);
end

% for i = 1:t
%     for j = 1:n
%     Y(i,j) = sum(S(T==UT(i)&ID==UID(j)));
%     end
% end

f1 = figure;

plot(UT,Y);
xlabel('仿真时间/秒');
ylabel('传输队列待发数据量/bit');
% hold on;
% for i = 1:n
%     text(UT(end),Y(end,i),num2str(UID(i)));
% end
% hold off;

f2 = figure;
plot(UT,Y2);
xlabel('仿真时间/秒');
ylabel('传输队列待发数据量/bit');


end
% 