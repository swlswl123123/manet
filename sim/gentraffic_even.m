function f2 = gentraffic_even(N,Ts,Te,S,R,centre_id,filename)

Np = R*N*(Te - Ts);


if centre_id > 0 %������
    L(:,1) = centre_id*ones(N-1,1);
    L(:,2) = [1:centre_id-1, centre_id+1:N]';
    L_tmp(:,1) = L(:,2);
    L_tmp(:,2) = L(:,1);
    L = [L;L_tmp];
else    %��״��
    L = npermutek(1:N,2);
end

Nl = size(L,1);

Li = randi(Nl,Np,1);

id = (1:Np)';
src = L(Li,1);
dst = L(Li,2);
t = (Te-Ts)*rand(Np,1)+Ts;
urg = rand(Np,1)>0.9;   % 10 percent of urgent traffic
len = S*ones(Np,1);


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

f1 = figure(1);

% mesh(usrc,udst,cnt);
% xlabel('Դ��ַ');
% ylabel('Ŀ�ĵ�ַ');
% zlabel('����');

f2 = figure(2);
histogram(t);
xlabel('����ʱ��/��');
ylabel('����');

csvwrite(filename,[id,src,dst,t,urg,len]);
end