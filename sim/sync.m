clc;clear;

N = 48; % 节点数
Nb = 40;    % 时隙数
Omega_tx = 4.5;  % 发射波束宽度
Omega_rx = 40.0; %　接收波束宽度
N_tx = 360/Omega_tx;    %全扫描所需次数
N_rx = 360/Omega_rx;    %全扫描所需次数
Ts = 90; %一个同步周期所需时间(s)
Nnd = 12; %一个通信时隙中最多链路发现时隙组数


[X,Y,D,f1] = mknet(N,8,4,4);    %生成随机网络(X,Y单位为通信半径)
print(f1,'-dpng','-r300','scan_topo.png');

T0 = Ts*rand(N,1);  %随机初始化节点参考时刻(ms)

DX =repmat(X',[N,1]) - repmat(X,[1,N]);% DX(i,j)的值表示Xj-Xi
DY = repmat(Y',[N,1]) - repmat(Y,[1,N]); % DY(i,j)的值表示Yj-Yi
Dist = sqrt(DX.^2+DY.^2); % Dist(i,j) 的值表示节点(i,j)之间的欧氏距离(以通信半径计算)


DD = zeros(N,N);       % 初始化由节点推算出的路由.


for l=1:2

%% 随机确定接收和发射节点

flag = randi(2,[N,1]);
flag = flag - 1;
F1 = repmat(flag,[1,N]);
F2 = repmat(flag',[N,1]);
F = F1 + F2;
F(F==2)=0; % F(i,j)==1 表示节点i与节点j是收发匹配的(i收j发或者i发j收)
F=F.*double(D);


% disp(sum(sum(F>0)));
% G = graph(F);
% figure(3);
% plot(G);


%% 确定发射节点的发射时刻和与之对应的发射角度
%% 在100s的发射周期内,每50ms即有一个链路发现时隙组,每个时隙组共有12个N时隙,共有24000次发射
%% 12*5*80*5 扫描角度计算周期
%% 扫描时刻计算周期 12*2000


t1 = ((1:12)-1)*0.75+0.05;


T_tx = repmat(t1,[N,Ts*20]);
for i = 1:Ts*20
    T_tx(:,(i-1)*12+1:(i-1)*12+12) = T_tx(:,(i-1)*12+1:(i-1)*12+12)+repmat((i-1)*50,[N,12]); 
end

% for i = 1:5
%     T_tx(:,(i-1)*Ts*64+1:(i-1)*Ts*64+Ts*64) = T_tx(:,(i-1)*Ts*64+1:(i-1)*Ts*64+Ts*64) + repmat((i-1)*10,[N,Ts*64]);    %加偏移 
% end

T_tx = T_tx + repmat(T0,[1,Ts*240]); %发射时刻(以绝对时间计)


%% 计算天线扫描方位角序列(16*20*2)

theta_seq = cell(40,1);
for i=1:20
        %主通信时隙 N1-N4 + S时隙N5-N16
        theta_seq{2*i-1} = [((4*(i-1)+1):(4*(i-1)+4))';zeros(8,1)];
        % 副通信时隙N1-N4 + S时隙N5-N16
        theta_seq{2*i} = [(mod((4*(i-1)+41):(4*(i-1)+44),80))';zeros(8,1)];
end

theta_seq = cell2mat(theta_seq);
theta_seq = mat2cell(theta_seq,repmat(80,[6,1]),1);

for i=1:6
    Vac = repmat(theta_seq{i},[1,80]);
    Sec = repmat(1:80,[80,1]);
    Fla = Vac == Sec;
    Compl = find(sum(Fla)==0);
    [rr,cc] = find(theta_seq{i}==0);
    for j=1:size(rr,1)
        theta_seq{i}(rr(j),cc(j))=Compl(j);
    end
end

theta_seq = cell2mat(theta_seq)';

return;


theta_tx = (repmat(theta_seq,[N,Ts/2])-1)/N_tx*2*pi;

% V_xtx = 
% V_ytx = 

%% 确定接收窗口和与之对应的接收角度
%% 在100s接收周期内,每50ms即由一个链路发现时隙组,每个时隙组有一个长度为9950us的接收窗口,共计2000个接收窗口
%% 接收角度计算周期5*80*5


T_rx1 =  repmat(((1:Ts*20)-1)*50+0.05,[N,1]); %接收窗口起始时刻(以节点时间计)
T_rx2 = repmat(((1:Ts*20)-1)*50+9.95,[N,1]);  %接收窗口终止时刻(以节点时间计)


T_rx_diff = cell(1,5);
for i=1:5
    T_rx_diff{i} = repmat((i-1)*10,[N,Ts*20/5]);
end

T_rx_diff = cell2mat(T_rx_diff);

T_rx1 = T_rx1 + repmat(T0,[1,Ts*20])+T_rx_diff;    %接收窗口起始时刻(以绝对时间计)
T_rx2 = T_rx2 + repmat(T0,[1,Ts*20])+T_rx_diff;    %接收窗口结束时刻(以绝对时间计)

theta_rx = cell(1,N_rx);

for i = 1:N_rx
    theta_rx{i} = repmat(-pi/2-(i-1)/N_rx*2*pi,[N,40]);
end

theta_rx = repmat(cell2mat(theta_rx),[1,5]);

[r,c] = find(F>0);

for i = 1:size(r,1)

    Nt = r(i);  %发射节点号
    Nr = c(i);  %接收节点号

    
    
    t_rx1 = repmat(T_rx1(Nr,:),[320*Ts,1]);
    t_rx2 = repmat(T_rx2(Nr,:),[320*Ts,1]);
    
    t_tx = repmat(T_tx(Nt,:)',[1,20*Ts]);
    t_tx = t_tx + Dist(Nt,Nr)*0.1; %%(加传播时延)
    t_match = (t_tx>t_rx1).*(t_tx < t_rx2); %%(t_match) 
    
    
    v_tr = [DX(Nt,Nr),DY(Nt,Nr)];
    v_rt = [DX(Nr,Nt),DY(Nr,Nt)];
    
    theta_rx_match = acos(v_rt*[cos(theta_rx(Nr,:));sin(theta_rx(Nr,:))]./Dist(Nr,Nt)) < Omega_rx/2/180*pi;
    theta_tx_match = acos(v_tr*[cos(theta_tx(Nt,:));sin(theta_tx(Nt,:))]./Dist(Nt,Nr)) < Omega_tx/2/180*pi;
    
    theta_match = double(theta_tx_match')*theta_rx_match;
    match = theta_match.*t_match;
    if sum(sum(match))>0
        DD(Nt,Nr)=1;
    end
    fprintf('[%d/%d][%d]---->[%d]:%d\n',i,size(r,1),Nt,Nr,DD(Nt,Nr));
end

end


DD = DD+DD';
DD(DD>0)=1;

f3 = plot_net(X,Y,DD);
print(f3,'-dpng','-r300','scan_link.png');
