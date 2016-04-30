#include<errno.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<memory.h>
#include<sys/socket.h> //socket()
#include<sys/ioctl.h>
#include<net/ethernet.h>
#include<net/if.h>
#include<netinet/in.h>
#include<netinet/ether.h>
#include<netinet/ip.h>
#include<netinet/tcp.h>
#include<arpa/inet.h>//inet_ntoa() struct in_addr


int getnumofbit( int num, int bit )
{
  int temp = num;
  unsigned int result = 0;
  result = num & ( 1 << ( 31-bit ) );
  if( result == 0 )
    return 0;
  else
    return 1;
}

int getDecnum( int num, int bit )
{
  int result = 0;
  int base = 2, increase = 1;
  int index = 0;
  /*
  注意：根据机器大端和小端的模式的不同，整数的位存储方式不同
  */
  for( index = bit - 1 ; index >= 0; index-- )
  {
    result += increase * getnumofbit( num, 16 + index );
    increase *= base;
  }

  return result;
}

int analyData( char *data )
{
  struct iphdr *ip;
  struct tcphdr *tcp;
  struct ether_header *ether;

  ether = (struct ether_header*)data;//若数据是从数据链路曾抓取的，那么就有这个以太网帧头
  printf("IP Packet Type:%d\n",ether->ether_type);

  ip = (struct iphdr*)( data + sizeof( struct ether_header ) );
  //ip = (struct iphdr *)data;
  printf("Protocal ---- %d\n", ip->protocol );
  printf("Source IP ---- %s\n", inet_ntoa( *( ( struct in_addr * )&ip->saddr ) ) );//将网络地址转换成“.”点隔的字符串格式。
  printf("Dest IP ---- %s\n", inet_ntoa( *( ( struct in_addr * )&ip->daddr ) ) );//将网络地址转换成“.”点隔的字符串格式。

  tcp = (struct tcphdr *) ( data + sizeof( *ip ) + sizeof( struct ether_header ) );//结构体
  printf("Source Port ---- %d\n", ntohs( tcp->source ) );
  printf("Dest Port ---- %d\n", ntohs( tcp->dest ) );

  int length = getDecnum( ntohs( *( (int *)tcp + 3 ) ), 4 ) * 4;
  char *temp_data = ( struct context *)( (int *)tcp +  length / 4  );
  if( len == ( length + 20  ) )
    printf("No Data. \n");
  else
    printf("Data ---- %s\n", temp_data );
  return 1;
}
int get_frame( char argv[] )
{
      unsigned char buffer[1024];
      struct sockaddr_in recvaddr;
      int count = 0;

      if( argv == NULL )
      {
        printf("Please enter the ecterface!\n");
        return -1;
      }
      else if( strcmp( argv, "eth0") != 0)
      {
        printf("Please enter eth0!\n");
        return -1;
      }
      memset( buffer, 0, sizeof(buffer) );

      //创建原始套接字
      //int sock_raw_fd = socket( PF_INET , SOCK_RAW, IPPROTO_TCP );//网络层抓取
      int sock_raw_fd = socket( PF_PACKET, SOCK_RAW, htons(ETH_P_ALL) );//数据链路层抓取
      if( sock_raw_fd < 0 )
      {
        printf("created raw socket error:%s !!!\n", strerror( errno ) );
        exit( 1 );
      }
      else
        printf("create raw socket %d sucess !!!\n", sock_raw_fd );

      // //设置网卡为混杂模式
      // struct ifreq ifr; //该结构体位于/usr/include/neether’ undeclared (OADCAST | IFF_RUNNING;
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
          printf("This frame get %d bytes !!!\n", len );
          buffer[len] = '\0';
          analyData( buffer );
          printf("Already get %d packet !!!\n", ++count );
          printf("\n");
        }
      }

      close( sock_raw_fd );
      return 0;
}

int main(int argc, char const *argv[]) {
  get_frame("eth0");
  return 0;
}
