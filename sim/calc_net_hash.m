function HASH = calc_net_hash(X,Y,Z,D,truncate,algorithm)
    Xd = typecast(X(:),'double');
    Yd = typecast(Y(:),'double');
    Zd = typecast(Z(:),'double');
    Dd = typecast(D(:),'uint8');
    S = [Xd;Yd;Zd;Dd];
    md = java.security.MessageDigest.getInstance(algorithm);    
    md.update(S);
    HASH = dec2hex(typecast(md.digest(),'uint8'))';
    HASH = HASH(:)';
    HASH = HASH(1:truncate);
 end