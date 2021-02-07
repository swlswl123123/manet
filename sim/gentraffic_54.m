function f2 = gentraffic_54(N,Ts,Te,R,centre_id,filename)
% clear all; close all; clc
% 参数
% N = 16; %节点个数
% Ts = 30; %开始时间
% Te = 50; %结束时间
% centre_id = 2; %星型网中心节点ID
% filename = ['../model/test.tx.csv'];

% 所有链路的组合

if centre_id > 0 %星型网
    L(:,1) = centre_id*ones(N-1,1);
    L(:,2) = [1:centre_id-1, centre_id+1:N]';
    L_tmp(:,1) = L(:,2);
    L_tmp(:,2) = L(:,1);
    L = [L;L_tmp];
else    %网状网
    L = npermutek(1:N,2);
end


Nl = size(L,1);

% 图像与情报类格式化信息

Np = R*4000*2*(Te - Ts)*0.5; %2路50%的持续时间，速率为4000次/s
Li = randi(Nl,Np,1); %随机挑选Np个组合
src1 = L(Li,1);
dst1 = L(Li,2);
t1 = (Te-Ts)*rand(Np,1)+Ts;
len1 = 2*1024*8*ones(Np,1); %数据包大小bit
urg1 = 0*ones(Np,1);

% 视频流

Np = 60*N*(Te - Ts); %N路连续，速率为60次/s
Li = randi(Nl,Np,1); %随机挑选Np个组合
src2 = L(Li,1);
dst2 = L(Li,2);
t2 = (Te-Ts)*rand(Np,1)+Ts;
len2 = 8*1024*8*ones(Np,1); %数据包大小bit
urg2 = 2*ones(Np,1);

% 无人机测控

Np = R*1020*N*(Te - Ts); %N路连续，速率为1020次/s
Li = randi(Nl,Np,1); %随机挑选Np个组合
src3 = L(Li,1);
dst3 = L(Li,2);
t3 = (Te-Ts)*rand(Np,1)+Ts;
len3 = 256*8*ones(Np,1); %数据包大小bit
urg3 = 3*ones(Np,1);

% 话音

Np = 4*N*(Te - Ts); %N路连续，速率为4次/s
Li = randi(Nl,Np,1); %随机挑选Np个组合
src4 = L(Li,1);
dst4 = L(Li,2);
t4 = poissrnd((Te+Ts)/2,[Np,1]);
t4 = (t4-min(t4))/(max(t4)-min(t4))*(Te-Ts) + Ts; %泊松分布
len4 = 600*ones(Np,1); %数据包大小bit
urg4 = 2*ones(Np,1);

src = [src1;src2;src3;src4];
dst = [dst1;dst2;dst3;dst4];
t = [t1;t2;t3;t4];
urg = [urg1;urg2;urg3;urg4];
len = [len1;len2;len3;len4];
id = (1:size(src,1))';

usrc = unique(src);
usrc = sort(usrc);
udst = unique(dst);
udst = sort(udst);


cnt = zeros(size(usrc,1),size(udst,1));

for i = 1:size(usrc,1)
for j = 1:size(udst,1)
    cnt(i,j) = sum(src == usrc(i) & dst == udst(j));
end
end

% f1 = figure(1);

% mesh(usrc,udst,cnt);
% xlabel('源地址');
% ylabel('目的地址');
% zlabel('包数');

f2 = figure(1);
histogram(t);
% hold on;
% plot(t1,len1);
% plot(gca,t2,len2);
% plot(gca,t3,len3);
% plot(gca,t4,len4);
hold off;

xlabel('仿真时间/秒');
ylabel('包数');

% writematrix([id,src,dst,t,urg,len],filename,);

fid=fopen(filename,'w+');
for i = 1:size(id,1)
    fprintf(fid,'%d,',id(i));
    fprintf(fid,'%d,',src(i));
    fprintf(fid,'%d,',dst(i));
    fprintf(fid,'%f,',t(i));
    fprintf(fid,'%d,',urg(i));
    fprintf(fid,'%d\n',len(i));
end
fclose(fid);

end
