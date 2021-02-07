function [f1,f2] = starplot(filename)
    F = csvread(filename);
    T  = F(:,1);
    ID = F(:,2);
    stem(T,ID, 'filled');
    set(get(gca, 'XLabel'), 'String', '时间/s');
    set(get(gca, 'YLabel'), 'String', '节点ID');
end