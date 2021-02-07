function [f1,f2,f3,BArmin,BArmax,B,X,Y,D] = balloc(N,Nb,Omega,HOP,W,H,Ne)

[X,Y,D,f1] = mknet(N,HOP,W,H);

Rc = zeros(Ne,1); %时隙收敛时长
BArate = zeros(Ne,1); %时隙分配率



for l = 1:Ne

DX =repmat(X',[N,1]) - repmat(X,[1,N]);% DX(i,j)的值表示Xj-Xi
DY = repmat(Y',[N,1]) - repmat(Y,[1,N]);
Dist = sqrt(DX.^2+DY.^2);

VX = reshape(DX,[N*N,1]);
VY = reshape(DY,[N*N,1]);
V = sqrt(VX'.^2+VY'.^2);

B = zeros(N,Nb);    %时隙占用向量
DD = zeros(size(D)) + eye(size(D));    %节点推算的网络拓扑


i = 1;
while true

    % 1/2概率随机收发
    flag = randi(2,size(X));
    flag = flag - 1;
    F1 = repmat(flag,[1,N]);
    F2 = repmat(flag',[N,1]);
    F = F1 + F2;
    F(F==2)=0;
    F = F + eye(size(F));
    
    
    for j = 1:360/Omega
        theta = pi/2 - (j-1)*Omega/180*pi;
        P = [cos(theta),sin(theta)];
        VISION = P*[VX';VY']./V;
        VISION = acos(VISION)/pi*180;
        VISION(isnan(VISION))=0;
        VISION(abs(VISION)>Omega/2)=0;
        VISION(VISION>0)=1;
        % disp(['gammar = ',num2str((j-1)*Omega),' num of VISION = ',num2str(sum(VISION))]);
        VISION = reshape(VISION,[N,N]);
        
        VISION = VISION.*F1.*(~F2).*(~DD).*D; %VISION(i,j)表示是否为i发j收且首次发现对方
        
        VD = VISION.*Dist;      %多发一收或多收一发时预约时隙时取距离最近者
        VD(VD==0)=Inf;

        [~,cc] = min(VD,[],2);

        VDD = zeros(size(VD));

        for k = 1:N
            VDD(k,cc(k)) = 1;
        end

        VISION = VISION.*VDD;

        [~,cc] = min(VD);
        VDD = zeros(size(VD));
        for k = 1:N
            VDD(cc(k),k) = 1;
        end

        VISION = VISION.*VDD;

        [rr,cc] = find(VISION==1); %预约时隙

        for k = 1:size(rr,1)

            BC = B(rr(k),:)+B(cc(k),:);

            if sum(B(rr(k),:)==cc(k))>1 || sum(B(cc(k),:)==rr(k))>1
                continue;
            end

            CH = nchoosek(1:Nb,2);

            [rrr,~] = find(sum(BC(CH),2)==0);

            if size(rrr,1)==0
                continue;
            end

            b = randi(size(rrr,1));

            B(rr(k),CH(rrr(b),1))=cc(k);
            B(rr(k),CH(rrr(b),2))=cc(k);
            B(cc(k),CH(rrr(b),1))=rr(k);
            B(cc(k),CH(rrr(b),2))=rr(k);
        end
        % return;
        
    end

    DD = DD+F.*D;
    DD(DD>0)=1;
    
    % return;
    



    % 判断拓扑是否收敛
    if D==DD
        % disp(['converge = ',num2str(i)]);
        % fprintf('时隙分配率 %.2f %%\n',sum(sum(B>0))/2/(sum(sum(D))-N)*100);
        % fprintf('路由收敛时长 %.2fs\n',i);
        BArate(l) = sum(sum(B>0))/2/(sum(sum(D))-N)*100;
        Rc(l) = i;

        break;
    end

    clc;
    fprintf('完成度:%d/%d\n',l,Ne);

    i=i+1;
end


% for i = 1:N
%     if sum(D(i,:))-1 > sum(B(i,:)>0)/2
%        fprintf('[%d]节点存在开路\n',i); 
%     end
% end



end

Dlink = zeros(N,N);

for i = 1:N
    for j = 1:N
        if sum(B(i,:)==j)>1
            Dlink(i,j)=1;
        end
    end
end

f2 = plot_net(X,Y,DD);

f3 = figure;
f3.PaperUnits = 'centimeters';
f3.PaperSize = [16,12];
f3.Units = 'centimeters';
f3.Position = [0,0,16,12];
histogram(BArate,0:100);
xlabel('\eta_B/%');
ylabel('N');

BArmin = min(BArate);
BArmax = max(BArate);

end