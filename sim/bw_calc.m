function bw = bw_calc(bw0)

    a = [cosd(bw0),0,sind(bw0)]';
    b = [cosd(bw0),sind(bw0),0]';
    
    bw = acos(a'*b)/pi*180;

end