clc;clear;
rng(1000);

[X,Y,D,f1] = mknet(48,8,4,4);

C = zeros(10000,1);

for j = 1:10000
i = 1;
DD = zeros(size(D));
while true
    
    flag = randi(2,size(X));
    flag = flag - 1;

    F1 = repmat(flag,[1,size(X,1)]);
    F2 = repmat(flag',[size(X,1),1]);
    F = F1 + F2;
    F(F==2)=0;
    F = F + eye(size(F));

    DD = DD+F.*D;
    DD(DD>0)=1;

    if D==DD
        % disp(['converge = ',num2str(i)]);
        break;
    end

    i=i+1;
end

C(j) = i; 

end

f3 = figure(3);
f3.Units = 'centimeters';
f3.PaperSize=[16,12];

histogram(C);
xlabel('T_{LD}/s');
ylabel('N');

print(f1,'-dpng','-r300','nd_topo.png');
print(f3,'-dpng','-r300','nd_dist.png');

fprintf('最小:%d\n',min(C));
fprintf('最大:%d\n',max(C));
fprintf('平均:%.2f\n',sum(C)/size(C,1));