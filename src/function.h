#ifndef FUNCTION_H
#define FUNCTION_H

#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <cmath>
#include <string.h>   
#include <errno.h>   
#include <stdlib.h>   
#include <unistd.h>   
#include <sys/types.h>   
#include <sys/socket.h>   
#include <netinet/in.h>   
#include <arpa/inet.h>   
#include <stdio.h>
#include <string.h>   
#include <errno.h>   
#include <stdlib.h>   
#include <unistd.h>   
#include <sys/types.h>   
#include <sys/socket.h>   
#include <netinet/in.h>   
#include <arpa/inet.h>   


#define DEST_PORT 7403   
#define DSET_IP_ADDRESS  "127.0.0.1"   

void sendUDP(double ,double);

void calculatePosition(pcl::PointCloud<pcl::PointXYZI>&, pcl::PointXYZI);

double twoPointsPosition(pcl::PointXYZI, pcl::PointXYZI);

void ifBarrier(pcl::PointCloud<pcl::PointXYZI>::Ptr & outPoints)
{
	int count_point=0;
	double x,y,z;
	pcl::PointXYZI min_y;
	pcl::PointCloud<pcl::PointXYZI> cloud;
	min_y.y=1000;
	for(int i=0;i<outPoints->points.size();i++)
	{
		x=outPoints->points[i].x;
		y=outPoints->points[i].y;
		z=outPoints->points[i].z;
		if(z>-0.2&&z<0.2&&x<0.9&&x>-0.9&&y<30&&y>2)
		{
		   cloud.push_back(outPoints->points[i]);
		   if(min_y.y>y)
			min_y=outPoints->points[i];
		
		}
		  //cout<<"x: "<<outPoints->points[i].x<<" y: "<<outPoints->points[i].y<<" z: "<<\
		outPoints->points[i].z<<endl;
	}
	calculatePosition(cloud, min_y);
	/*ROS_INFO("counts of points : %d",count_point);
	if(count_point>20)
	    ROS_INFO("danger! : %d",count_point);
*/
}

void KMeans(pcl::PointCloud<pcl::PointXYZI>::Ptr & cloud)
{

}


void calculatePosition(pcl::PointCloud<pcl::PointXYZI> & cloud, pcl::PointXYZI min_y)
{
	double x,y,z;
	int counts=0;
 	for(int i=0;i<cloud.points.size();i++)
	{
		if(twoPointsPosition(cloud.points[i], min_y)<1)
		{
			counts++;
			
		}				

			//ROS_INFO("%lf",twoPointsPosition(cloud.points[i], min_y));
	}

	ROS_INFO("%d",counts);
	if(counts>30)
	{	ROS_INFO("barrier is in (%lf, %lf, %lf)", min_y.x,min_y.y,min_y.z);
		sendUDP(min_y.x,min_y.y);
	}
	
}

double twoPointsPosition(pcl::PointXYZI a, pcl::PointXYZI b)
{
	return sqrt(pow(a.x-b.x,2) + pow(a.y-b.y,2) + pow(a.z-b.z,2));	
}

void sendUDP(double x, double y)
{
  /* socket文件描述符 */  
  int sock_fd;  
  
  /* 建立udp socket */  
  sock_fd = socket(AF_INET, SOCK_DGRAM, 0);  
  if(sock_fd < 0)  
  {  
    ROS_INFO("socket");  
    exit(1);  
  }  
    
  /* 设置address */  
  struct sockaddr_in addr_serv;  
  int len;  
  memset(&addr_serv, 0, sizeof(addr_serv));  
  addr_serv.sin_family = AF_INET;  
  addr_serv.sin_addr.s_addr = inet_addr(DSET_IP_ADDRESS);  
  addr_serv.sin_port = htons(DEST_PORT);  
  len = sizeof(addr_serv);  
  
    
  int send_num;  
  int recv_num;  
  char send_buf[20];  
  char recv_buf[20];
  sprintf(send_buf, "%lf %lf", x,y);  
      
  printf("client send: %s\n", send_buf);  
    
  send_num = sendto(sock_fd, send_buf, strlen(send_buf), 0, (struct sockaddr *)&addr_serv, len);  
    
  if(send_num < 0)  
  {  
    perror("sendto error:");  
    exit(1);  
  }  
    
  /*recv_num = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&addr_serv, (socklen_t *)&len);  
    
  if(recv_num < 0)  
  {  
    perror("recvfrom error:");  
    exit(1);  
  }  
    
  recv_buf[recv_num] = '\0';  
  printf("client receive %d bytes: %s\n", recv_num, recv_buf);  
    */
  close(sock_fd);  
}

#endif
