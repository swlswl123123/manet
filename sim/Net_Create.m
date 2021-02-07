close all;
clc;
clear;

% HOP_MAX=1;

X_min = 0;      %节点最小横坐标
X_max = 2.8;      %节点最大横坐标
Y_min = 0;      %节点最小纵坐标
Y_max = 2.8;      %节点最大纵坐标
try_count=0;
HOP_MAX=4;
Nnode_t = 100;    % 节点总数
Rate_t = [7.5 7.5 7.5 5 5];    % 节点总数，随机均??分布

for jj=1:1
    % X_max =HOP_MAX*Rate_t(jj);
    % Y_max =HOP_MAX*Rate_t(jj);
    for ii=1:1
        Nnode=Nnode_t;
        close all
        net_count=0;
        
        while net_count<1
            try_count=try_count+1;
            NODES = cell(Nnode,1);    %节点存储单元(数组) 每个cell中保存一个节点
            
            X = (X_max-X_min)*rand(Nnode,1)+X_min;
            Y = (Y_max-Y_min)*rand(Nnode,1)+Y_min;
            NTR = double(3600000*rand(Nnode,1));
            D = sqrt((((repmat(X,[1,Nnode]) - repmat(X,[1,Nnode])').^2) + ((repmat(Y,[1,Nnode]) - repmat(Y,[1,Nnode])').^2)));
            
            disp(['HOP_MAX = ',num2str(HOP_MAX)]);
            disp(['Nnode = ',num2str(Nnode)]);
            
            
            
            D(D>1)=0;
            D(D>0)=1;
            
            [p,q,v]=find(D);
            b=sparse(p,q,v,Nnode,Nnode);
            hop_count=zeros(Nnode,Nnode);
            for i=1:Nnode
                for j=1:Nnode
                    [hop_count(i,j),y,z]=graphshortestpath(b,i,j,'Directed',false);
                end
            end
            hop_max=max(max(hop_count));
            disp(['hop_max = ',num2str(hop_max)]);
            % [node_beigin,node_end]=find(hop_count==hop_max);
            if(hop_max==HOP_MAX)
                net_count=net_count+1;
                % nodebn=[node_beigin,node_end];
                % save([num2str(HOP_MAX),'_node_',num2str(net_count),'.txt'],'nodebn','-ascii');
                num_mobile=randperm(Nnode/4+1,1)-1;%???????
                INDEX_NODE = uint16(randperm(Nnode,num_mobile)')%????????
                INDEX_TRJ = uint16(randperm(Nnode,num_mobile)')%?????????
                TRJ = 64*ones(1,Nnode)';
                TRJ(INDEX_NODE)=INDEX_TRJ;
                G=graph(triu(D==1,1),'upper')
                
                
                A = adjacency(G);
                
                
                
                h1=figure;
                scatter(X,Y,'filled','r');
%                 grid on;
                hold on;
                
                     
                names = cell(Nnode,1);
                IDS_ORIG = uint16(randperm(100,Nnode)');
                for i = 1:Nnode
                    names{i} = num2str(IDS_ORIG(i));
                    %  names{i} = num2str(i);
                end
                text(X,Y,names, 'FontSize',11);
                print(h1,'-dpng','-r300',[num2str(HOP_MAX),'_',num2str(Nnode),'_',num2str(net_count),'_scene','.png']);
                
                h2=figure;
                h3=plot(G);
                
                for i = 1:Nnode
                    % labelnode(h3,i,num2str(i));
                    labelnode(h3,i,num2str(IDS_ORIG(i)));
                end
                print(h2,'-dpng','-r300',[num2str(HOP_MAX),'_',num2str(Nnode),'_',num2str(net_count),'_topo','.png']);
                
                fid=fopen([num2str(HOP_MAX),'_',num2str(Nnode),'_',num2str(net_count),'.csv'],'w');
                for i = 1:Nnode
                    fprintf(fid,'%s,',names{i});
                    fprintf(fid,'%d,',IDS_ORIG(i));
                    fprintf(fid,'%f,',NTR(i));
                    fprintf(fid,'%f,',X(i));
                    fprintf(fid,'%f,',Y(i));
                    fprintf(fid,'%d\n',TRJ(i));
                end
                
                fclose(fid);
                
            end
        end
        
    end
end
