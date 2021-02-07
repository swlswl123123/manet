function [] = gentraffic_new(Ts,Te,netname)

    str = netname(1:2);
    % chain topology
    if str == 'CH'
        N = 4;
        L = npermutek(1:N,2);
        Nl = size(L,1);
        
        % image
        R1 = 4000;
        Np1 = R1*N*(Te - Ts);
        Li1 = randi(Nl,Np1,1); % random select Np1 src and dst
        id1 = (1:Np1)';
        src1 = L(Li1,1);
        dst1 = L(Li1,2);
        t1 = (Te-Ts)*rand(Np1,1)+Ts;
        len1 = 2000*8*ones(Np1,1);
        urg1 = ones(Np1,1)*2;

        % video
        R2 = 120;
        Np2 = R2*N*(Te - Ts);
        Li2 = randi(Nl,Np2,1);
        id2 = (1:Np2)' + id1(end);
        src2 = L(Li2,1);
        dst2 = L(Li2,2);  
        t2 = (Te-Ts)*rand(Np2,1)+Ts;
        len2 = 8000*8*ones(Np2,1);
        urg2 = ones(Np2,1)*1;

        % detect
        R3 = 6576;
        Np3 = R3*N*(Te - Ts);
        Li3 = randi(Nl,Np3,1);
        id3 = (1:Np3)' + id2(end);
        src3 = L(Li3,1);
        dst3 = L(Li3,2);  
        t3 = (Te-Ts)*rand(Np3,1)+Ts;
        len3 = 63*8*ones(Np3,1);
        urg3 = ones(Np3,1)*1;

        % measure
        R4 = 25;
        Np4 = R4*(Te - Ts);
        Li4 = randi(Nl,Np4,1);
        id4 = (1:Np4)' + id3(end);
        src4 = L(Li4,1);
        dst4 = L(Li4,2);  
        t4 = (Te-Ts)*rand(Np4,1)+Ts;
        len4 = 64*8*ones(Np4,1);
        urg4 = ones(Np4,1)*3;

        % combine
        id = [id1;id2;id3;id4];
        src = [src1;src2;src3;src4];
        dst = [dst1;dst2;dst3;dst4];
        t = [t1;t2;t3;t4];
        len = [len1;len2;len3;len4];
        urg = [urg1;urg2;urg3;urg4];
        csvwrite(['../model/', netname,'.tx.csv'],[id,src,dst,t,urg,len]);
    elseif str == 'NE'
        N = 16;
        L = npermutek(1:N,2);
        Nl = size(L,1);
        
        % image
        R1 = 4000;
        Np1 = R1*3*(Te - Ts);
        Li1 = randi(Nl,Np1,1); % random select Np1 src and dst
        id1 = (1:Np1)';
        src1 = L(Li1,1);
        dst1 = L(Li1,2);
        t1 = (Te-Ts)*rand(Np1,1)+Ts;
        len1 = 2000*8*ones(Np1,1);
        urg1 = ones(Np1,1)*2;

        % video
        R2 = 60;
        Np2 = R2*16*(Te - Ts);
        Li2 = randi(Nl,Np2,1); % random select Np1 src and dst
        id2 = (1:Np2)' + id1(end);;
        src2 = L(Li2,1);
        dst2 = L(Li2,2);
        t2 = (Te-Ts)*rand(Np2,1)+Ts;
        len2 = 8000*8*ones(Np2,1);
        urg2 = ones(Np2,1)*1;

        % detect
        R3 = 200;
        Np3 = R3*16*(Te - Ts);
        Li3 = randi(Nl,Np3,1);
        id3 = (1:Np3)' + id2(end);
        src3 = L(Li3,1);
        dst3 = L(Li3,2);  
        t3 = (Te-Ts)*rand(Np3,1)+Ts;
        len3 = 63*8*ones(Np3,1);
        urg3 = ones(Np3,1)*1;  
        
        % measure
        R4 = 50;
        Np4 = R4*15*(Te - Ts);
        Li4 = randi(Nl,Np4,1);
        id4 = (1:Np4)' + id3(end);
        src4 = L(Li4,1);
        dst4 = L(Li4,2);  
        t4 = (Te-Ts)*rand(Np4,1)+Ts;
        len4 = 256*8*ones(Np4,1);
        urg4 = ones(Np4,1)*3;  
        
        % combine
        id = [id1;id2;id3;id4];
        src = [src1;src2;src3;src4];
        dst = [dst1;dst2;dst3;dst4];
        t = [t1;t2;t3;t4];
        len = [len1;len2;len3;len4];
        urg = [urg1;urg2;urg3;urg4];
        csvwrite(['../model/', netname,'.tx.csv'],[id,src,dst,t,urg,len]);
    else
        N = 16;
        centre_id = 1;
        L(:,1) = centre_id*ones(N-1,1); % center id 1
        L(:,2) = [1:centre_id-1, centre_id+1:N]';
        L_tmp(:,1) = L(:,2);
        L_tmp(:,2) = L(:,1);
        L = [L;L_tmp];
        Nl = size(L,1);

        % image
        R1 = 4000;
        Np1 = R1*3*(Te - Ts);
        Li1 = randi(Nl,Np1,1); % random select Np1 src and dst
        id1 = (1:Np1)';
        src1 = L(Li1,1);
        dst1 = L(Li1,2);
        t1 = (Te-Ts)*rand(Np1,1)+Ts;
        len1 = 2000*8*ones(Np1,1);
        urg1 = ones(Np1,1)*2;

        % video
        R2 = 60;
        Np2 = R2*15*(Te - Ts);
        Li2 = randi(Nl,Np2,1); % random select Np1 src and dst
        id2 = (1:Np2)' + id1(end);;
        src2 = L(Li2,1);
        dst2 = L(Li2,2);
        t2 = (Te-Ts)*rand(Np2,1)+Ts;
        len2 = 8000*8*ones(Np2,1);
        urg2 = ones(Np2,1)*1;

        % detect
        R3 = 2740;
        Np3 = R3*15*(Te - Ts);
        Li3 = randi(Nl,Np3,1);
        id3 = (1:Np3)' + id2(end);
        src3 = L(Li3,1);
        dst3 = L(Li3,2);  
        t3 = (Te-Ts)*rand(Np3,1)+Ts;
        len3 = 63*8*ones(Np3,1);
        urg3 = ones(Np3,1)*1;  
        
        % measure
        R4 = 50;
        Np4 = R4*15*(Te - Ts);
        Li4 = randi(Nl,Np4,1);
        id4 = (1:Np4)' + id3(end);
        src4 = L(Li4,1);
        dst4 = L(Li4,2);  
        t4 = (Te-Ts)*rand(Np4,1)+Ts;
        len4 = 256*8*ones(Np4,1);
        urg4 = ones(Np4,1)*3;  
        
        % combine
        id = [id1;id2;id3;id4];
        src = [src1;src2;src3;src4];
        dst = [dst1;dst2;dst3;dst4];
        t = [t1;t2;t3;t4];
        len = [len1;len2;len3;len4];
        urg = [urg1;urg2;urg3;urg4];
        csvwrite(['../model/', netname,'.tx.csv'],[id,src,dst,t,urg,len]);        
    end
end

