function save_net_csv(lat,lon,alt,filename)
    fid=fopen(filename,'w+');
    for i = 1:size(lat,1)
        fprintf(fid,'%d,',i);
        fprintf(fid,'%f,',lon(i));
        fprintf(fid,'%f,',lat(i));
        fprintf(fid,'%f\n',alt(i));
    end
    fclose(fid);
end