clc;clear;close all;

rng(12345);


N = [48,20]';
Nb = [40,40]';
Omega = [4.5,4.5]';
HOP = [8,1]';
W = [4,1]';
H = [4,1]';
Ne = [10000,10000]';

for i = 2:2
    [f1,f2,f3] = balloc(N(i),Nb(i),Omega(i),HOP(i),W(i),H(i),Ne(i));
    print(f1,'-dpng','-r300',['base_alloc_topo_',num2str(N(i)),'_',num2str(HOP(i)),'_',num2str(Ne(i)),'.png']);
    print(f2,'-dpng','-r300',['base_alloc_link_',num2str(N(i)),'_',num2str(HOP(i)),'_',num2str(Ne(i)),'.png']);
    print(f3,'-dpng','-r300',['base_alloc_dist_',num2str(N(i)),'_',num2str(HOP(i)),'_',num2str(Ne(i)),'.png']);
end





