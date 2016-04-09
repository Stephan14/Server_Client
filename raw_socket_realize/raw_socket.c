#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<net/ethernet.h>
#include<sys/socket.h> //socket()
#include<sys/ioctl.h>
#include<netinet/ether.h>
#include<netinet/ip.h>
#include<netinet/tcp.h>
#include<net/if.h>
#include<memory.h>
#include<errno.h>
#include<unistd.h>
#include<arpa/inet.h>//inet_ntoa() struct in_addr

int analyData( char *data )
{
  struct iphdr *ip;
  struct tcphdr *tcp;
  struct ether_header *ehter;

  //ether=(struct ether_header*)data;//若数据是从数据链路曾抓取的，那么就有这个以太网帧头
  //printf(" data packet type:%d/n",ether->ether_type);
  //ip=(struct iphdr*)(data+sizeof(struct ether_header));
  ip = (struct iphdr *)data;
  printf("Protocal ---- %d\n", ip->protocol );
  printf("Source IP ---- %s\n", inet_ntoa( *( ( struct in_addr * )&ip->saddr ) ) );//将网络地址转换成“.”点隔的字符串格式。
  printf("Dest IP ---- %s\n", inet_ntoa( *( ( struct in_addr * )&ip->daddr ) ) );//将网络地址转换成“.”点隔的字符串格式。

  tcp = (struct tcphdr *) ( data + sizeof( *ip ) );//结构体
  printf("Source IP ---- %d\n", ntohs( tcp->source ) );
  printf("Dest IP ---- %d\n", ntohs( tcp->dest ) );

  return 1;
}
int main(int argc,char *argv[])
{
      unsigned char buffer[1024];
      struct sockaddr_in recvaddr;
      int count = 0;

      if( argc != 2 )
      {
        printf("please enter the ecterface!");
        exit( 1 );
      }
      memset( buffer, 0, sizeof(buffer) );
      //创建原始套接字
      int sock_raw_fd = socket( PF_INET , SOCK_RAW, IPPROTO_TCP );//网络层抓取
      //int sock_raw_fd = socket(PF_PACKET,SOCK_RAW,ETH_P_IP)//数据链路层抓取
      if( sock_raw_fd < 0 )
      {
        printf("created raw socket error:%s !!!\n", strerror( errno ) );
        exit( 1 );
      }
      else
        printf("create raw socket %d sucess !!!\n", sock_raw_fd );

      // //设置网卡为混杂模式
      // struct ifreq ifr; //该结构体位于/usr/include/net/if.h
      // strcpy( ifr.ifr_name, "eth0" );
      // ifr.ifr_flags = IFF_UP | IFF_PROMISC | IFF_BROADCAST | IFF_RUNNING;
      // if( ioctl( sock_raw_fd, SIOCSIFFLAGS, &ifr ) == -1 )//设置混杂模式
      // {
      //   printf("set %s promisc mode failed !!!\n", ifr.ifr_name );
      //   exit( 1 );
      // }
      // else
      //   printf("set %s promisc mode sucess !!!\n", ifr.ifr_name );

      int recv_len = sizeof( struct sockaddr_in );

      while(1)
      {
        int len = recvfrom( sock_raw_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&recvaddr, &recv_len );
        if( len > 0 )
        {
          printf("获取%d字节数据!!!\n", len );
          buffer[len] = '\0';
          analyData( buffer );
          printf("Already get %d packet !!!\n", ++count );
        }
      }

     return 0;
}
