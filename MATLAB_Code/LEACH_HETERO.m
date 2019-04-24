function [num_cls,cl_head,cl_mem,prob_s]=LEACH_HETERO(NDid,Xc,Yc,Pwr,n,sinkx,sinky,grid_size,sinkid)

xm=grid_size;
ym=grid_size;
p=0.1;
%Values for Hetereogeneity
%Percentage of nodes than are advanced
m=0.1;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%                           LEACH                               %%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
for h=1:1

    Et=0;
    for i=1:1:n      
        S(i).xd=Xc(i);
        XR(i)=S(i).xd;       
        S(i).yd=Yc(i);
        YR(i)=S(i).yd;
        distance=sqrt( (S(i).xd-(sinkx) )^2 + (S(i).yd-(sinky) )^2 );
        S(i).distance=distance;
        S(i).G=0;
        %initially there are no cluster heads only nodes
        S(i).type='N';      
        S(i).E=Pwr(i);
        Et=Et+S(i).E;
        
        %initially there are no cluster heads only nodes
        
        temp_rnd0=i;
        %Random Election of Normal Nodes
        if (temp_rnd0>=m*n+1)
            S(i).type='N';            
        end
        %Random Election of Advanced Nodes
        if (temp_rnd0<m*n+1)
            S(i).type='A';           
        end
    end
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
       r=1;
        if(mod(r, round(1/p) )==0)
            for i=1:1:n
                S(i).G=0;
                S(i).cl=0;
            end
        end
        
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        for i=1:1:n
            %checking if there is a dead node
            if S(i).E>0
                S(i).type='N';
            end
        end
     
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%       
        cluster=1;
        for i=1:1:n
            if(S(i).E>0)
                temp_rand=rand;
                if ( (S(i).G)<=0)
                    
                    %Election of Cluster Heads for normal nodes
                    if ( temp_rand <= ( p/ ( 1 - p * mod(r,round(1/p)) )) )
                        
                        S(i).type='C';
                        S(i).G=round(1/p)-1;
                        C(cluster).xd=S(i).xd;
                        C(cluster).yd=S(i).yd;
                        %plot(S(i).xd,S(i).yd,'k*');
                        
                        distance=sqrt( (S(i).xd-(sinkx) )^2 + (S(i).yd-(sinky) )^2 );
                        C(cluster).distance=distance;
                        C(cluster).id=i;
                        X(cluster)=S(i).xd;
                        Y(cluster)=S(i).yd;
                        cluster=cluster+1;                 
                        
                        
                    end
                    
                end
            end
        end
        C(cluster).id=cluster;
        C(cluster).xd=sinkx;
        C(cluster).yd=sinky;
        C(cluster).distance=0;
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        %Election of Associated Cluster Head for Normal Nodes
        cl_mem=zeros(n,1);
        for i=1:1:n
            cl_mem(i)=sinkid;
            if ( S(i).type=='N' && S(i).E>0 )
                 min_dis=sqrt( (S(i).xd-sinkx)^2 + (S(i).yd-sinky)^2 );
                 min_dis_cluster=cluster;                 
                if(cluster-1>=1)                   
                    for c=1:1:cluster-1
                        temp=min(min_dis,sqrt( (S(i).xd-C(c).xd)^2 + (S(i).yd-C(c).yd)^2 ) );
                        if ( temp<min_dis )
                            min_dis=temp;
                            min_dis_cluster=c;
                            cl_mem(i)=C(c).id;
                        end
                    end       

                end
                S(i).min_dis=min_dis;
                S(i).min_dis_cluster=min_dis_cluster;                
            end
        end
        
   clf;     
%     fig1=gcf;
%     figure(fig1);
%     subplot(1,2,1);
    plot(sinkx,sinky,'o', 'MarkerSize', 5, 'MarkerFaceColor', 'r');
    text(sinkx+(grid_size/100),sinky,'sink');
    axis([0 xm 0 ym]);
    hold on;
    for i=1:1:n
%         figure(fig1);
%         subplot(1,2,1);
        plot(S(i).xd,S(i).yd,'o', 'MarkerSize', 5, 'MarkerFaceColor', 'g');
        text(S(i).xd+(grid_size/100),S(i).yd,num2str(NDid(i)));
        hold on;
        for k=1:1:cluster-1
            if(i==C(k).id)
%                 figure(fig1);
%                 subplot(1,2,1);
                plot(S(i).xd,S(i).yd,'o', 'MarkerSize', 5, 'MarkerFaceColor', 'b');
                text(S(i).xd+(grid_size/100),S(i).yd,num2str(NDid(i)));
                axis([0 xm 0 ym]);                
                hold on;
            end
        end
    end
    X(cluster)=sinkx;
    Y(cluster)=sinky;
    if(numel(X)>=4)
            warning('OFF');
            [vx,vy]=voronoi(X(:),Y(:));
            plot(X,Y,'r+',vx,vy,'m-');
            %hold on;
            voronoi(X,Y);
            title 'LEACH Cluster formation';
            xlabel('X Coordinates')
            ylabel('Y Coordinates')
    end
   
    
    
%     %3D Plotting starts here    
%     new_p=zeros(n,1);
%      prob_s=zeros(n,1);
%     for i=1:n
%         new_p(i)=S(i).E;
%         prob_s(i)=S(i).G;
%     end
%     new_p=new_p-2;
%     figure(fig1);
%     subplot(1,2,2);
%     tri = delaunay(Xc,Yc);
%     trisurf(tri,Xc,Yc,new_p);
%     axis([0 max(Xc) 0 max(Yc) 0 max(new_p)*2]);
%     title 'Energy Consumption';
%     xlabel('Sensor X position')
%     ylabel('Sensor Y position')
%     zlabel('Energy Consumed (J)')
%     lighting phong
%     shading interp
%     colorbar EastOutside
%     %3D Plotting ends here

    %Output to NetSim
    num_cls=cluster;%includes sinknode
    cl_head=zeros((cluster),1);
   
    for i=1:cluster-1
        cl_head(i)=C(i).id;
    end
    cl_head(cluster)=sinkid;
    
    %Log file for debugging
    %save LEACH_HETEROGENEOUS.mat
end