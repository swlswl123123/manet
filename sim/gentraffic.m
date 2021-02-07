function gentraffic(N,Ts,Te,S,R,s,d,netname)


Np = R*1*(Te - Ts);


L = npermutek(1:N,2);
Nl = size(L,1);

% Li = randi(Nl,Np,1);

id = (1:Np)';
src = s*ones(Np,1);
dst = d*ones(Np,1);
t = (Te-Ts)*rand(Np,1)+Ts;
urg = zeros(Np,1);
len = S*ones(Np,1);

csvwrite([netname,'.tx.csv'],[id,src,dst,t,urg,len]);
end