function save_trj_csv(lat,lon,alt,time_step,filename)
    fid=fopen(filename,'w+');
    fprintf(fid,'%d\n',size(lat,1));
    fprintf(fid,'locale: C\n');
    fprintf(fid,'%f\n',time_step);
    fprintf(fid,'Degrees\n');
    fprintf(fid,'fixed\n');
    for i = 1:size(lat,1)
        fprintf(fid,'%f,',lon(i));
        fprintf(fid,'%f,',lat(i));
        fprintf(fid,'%f\n',alt(i));
    end
    fclose(fid);
end