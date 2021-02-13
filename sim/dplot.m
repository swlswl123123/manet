function [f1,f2,f3,f4,f5,f6] = dplot(netname)
    
    fname_delay = [netname,'.delay.csv'];
    fname_send = [netname,'.send.csv'];
    fname_relay = [netname,'.relay.csv'];
    
    M = csvread(fname_delay);
    M2 = csvread(fname_send);
    
    TS = M2(:,1);
    TR = M(:,1);
    
    ID = M2(:,2);
    URG = M2(:,3);
    
    T = [TS;TR];
    UT = unique(T);
    UT = sort(UT);
    
    UD = unique(M(:,3));
    UD = sort(UD);
    
    f1 = figure;
    
    DELAY = zeros(size(UD,1),1);
    for i = 1:size(UD,1)
    DELAY(i) = sum(M(:,3)<=UD(i))/size(M,1)*100;
    end
    
    plot(UD,DELAY);
    
    
    xlabel('端-端时延 t/s');
    ylabel('%');
    
    
    TX = zeros(size(UT,1),1);
    
    for i =1:size(UT,1)
        TX(i) = sum(TS<=UT(i));
    end
    
    RX = zeros(size(UT,1),1);
    
    for i = 1:size(UT,1)
        RX(i) = sum(TR<=UT(i));
    end
    
    f2 = figure;
    P = RX./TX*100;
    P(P > 100) = 100;
    plot(UT,P);
    xlabel('仿真时间/秒');
    ylabel('成功送达率 %');
    
    f3 = figure;
    DATA = zeros(size(UT,1),1);
    
    for i = 1:size(UT,1)
        DATA(i) = sum(TR<=UT(i))*12000/(UT(i)-UT(1))/1e6;
    end
    
    plot(UT,DATA);
    ylabel('端-端数据传输速率 (Mb/s)');
    xlabel('仿真时间/秒');
    
    f4 = figure;
    
    Mrelay = csvread(fname_relay);
    
    Mrelay(Mrelay(:,2)==Mrelay(:,4),:) = [];    % 不统计发送端
    
    
    Trelay = [Mrelay(:,1);TR];
    IDrelay = Mrelay(:,3);
    IDrecv = M(:,2);
    
    UTrelay = unique(Trelay);
    UIDrecv = unique(IDrecv);
    URGrecv = zeros(size(UIDrecv,1),1);
    for i = 1:size(UIDrecv,1)
        tt = URG(ID==UIDrecv(i));
        URGrecv(i) = tt(1);
    end
    
    UT = sort(unique([UTrelay;TS]));
    
    
    DATAtotal = zeros(size(UT,1),1);
    
    for i = 1:size(UT,1)
    DATAtotal(i) = sum(Trelay<=UT(i))*12000/(UT(i)-UT(1))/1e6;
    end
    
    plot(UT,DATAtotal);
    ylabel('网络吞吐量 (Mb/s)');
    xlabel('仿真时间/秒');
    
    f5 = figure;
    f5.PaperUnits = 'centimeters';
    f5.PaperSize = [16,12];
    
    
    HOPS = zeros(size(UIDrecv,1),1);
    DELAY_PER_HOP = zeros(size(UIDrecv,1),1);
    
    for i = 1:size(UIDrecv,1)
        delay = M(M(:,2)==UIDrecv(i),3);
        delay = delay(1);
        HOPS(i) = sum(IDrelay==UIDrecv(i))+1;
        DELAY_PER_HOP(i) = delay/HOPS(i);
    end
    
    UDPH = unique(DELAY_PER_HOP);
    UDPH = sort(UDPH);
    PDF_UDPH  = zeros(size(UDPH,1),1);
    PDF_UDPH_URG = zeros(size(UDPH,1),1);
    
    for i = 1:size(UDPH,1)
    PDF_UDPH(i) = sum(DELAY_PER_HOP<=UDPH(i))/size(DELAY_PER_HOP,1)*100;
    PDF_UDPH_URG(i) = sum(DELAY_PER_HOP<=UDPH(i) & URGrecv==1)/sum(URGrecv)*100;
    end
    
    UDPH = log10(UDPH/0.2);
    
    
    plot(UDPH,PDF_UDPH,UDPH,PDF_UDPH_URG);
    xt = xticks;
    xt = 50*10.^xt;
    
    xl = cell(size(xt,1),size(xt,2));
    
    for i = 1:size(xt,2)
        xl{i} = num2str(xt(i));
    end
    
    xticklabels(xl);
    
    legend({'所有业务','紧急业务'},'location','southeast');
    
    xlabel('单跳时延 \tau_{hop}</ms');
    ylabel('%');
    
    f6 = figure;
    f6.PaperUnits = 'centimeters';
    f6.PaperSize = [16,12];
    histogram(DELAY_PER_HOP*1000, [0:10:1000]);
    
    xlabel('单跳时延/ms');
    ylabel('接收的数据包数目');
    
    end