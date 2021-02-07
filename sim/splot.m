function [f1,f2,f3] = splot(netname)

F = csvread([netname,'.sync.csv']);

l = size(F,1);

T  = F(:,1);
ID = int64(F(:,2));
REF = int64(F(:,3));
CLOCK = uint64(F(:,4));
CLOCK = mod(CLOCK,2000000000);


UID = unique(ID);
UT = unique(T);
MIN_ID = min(UID);

n = size(UID,1);
t = size(UT,1);

N = zeros(n,1);

Y = zeros(t-1,1);

for i = 1:t-1
    Y(i) = sum(REF == MIN_ID & (T <= UT(i)))/n*100;
end

% for i = 1:t
%     for j = 1:n
%     Y(i,j) = sum(NB(T==UT(i)&ID==UID(j)));
%     end
% end

f1 = figure;

plot(UT(1:end-1),Y);
xlabel('仿真时间/s');
ylabel('时间同步率/%');


f2 = figure;

% stairs(T(ID==1),CLOCK(ID==1));
% ax = gca;
hold on;
for i = 1:n
    stairs(gca,T(ID==UID(i)),CLOCK(ID==UID(i)));
end
hold off;

xlabel('仿真时间/s');
ylabel('参考时钟/ns');

f3 = figure;

REF_CLOCK = CLOCK(ID==MIN_ID);
REF_CLOCK = REF_CLOCK(1);

SYNC_CLK = zeros(n,1);

for i = 1:n
    sync_clk = CLOCK(ID==UID(i));
    sync_clk = sync_clk(end);
    SYNC_CLK(i) = sync_clk;
end

histogram(SYNC_CLK-double(REF_CLOCK),-10:1:10);

xlabel('时钟同步误差/ns');
ylabel('节点数');

end