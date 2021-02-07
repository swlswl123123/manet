function [X,Y,D] = mkgrid(m,n)
%myFun - Description
%
% Syntax: output = mknet(N,hop,width,height)
%
% Long description

N = m*n;
[X,Y] = meshgrid(1:m,1:n);
X = X(:) - m/2;
Y = Y(:) - n/2;
X = X*0.65;
Y = Y*0.65;


D = sqrt((((repmat(X,[1,N]) - repmat(X,[1,N])').^2) + ((repmat(Y,[1,N]) - repmat(Y,[1,N])').^2)));
D(D==0)=1;
D(D>1)=0;
D(D>0)=1;


D = uint8(D);

% f = figure;
% f.Units = 'centimeters';
% f.PaperSize=[8,6];
% scatter(X,Y,'filled','r');
% axis equal;

% names = cell(N,1);
% for i =1:N
% names{i} = num2str(i);
% end

% text(X,Y,names,'FontSize',12);

% hold on;
% for i=1:N-1
%     for j = i+1:N
%         if D(i,j)==1
%         line([X(i),X(j)]',[Y(i),Y(j)]','LineStyle',':');
%         end
%     end
% end
% hold off;

% xlabel('x/D_{hop}');
% ylabel('y/D_{hop}');

% G=graph(triu(D==1,1),'upper');
% figure(2);
% h2 = plot(G);

% for i=1:N
%     labelnode(h2,i,num2str(i));
% end

    
end