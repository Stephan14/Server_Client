#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<memory.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

#define SERVER_PORT 8000


int main(int argc, char const *argv[]) {
  int sockfd;
  struct sockaddr_in serv_addr;

  //创建socket套接字
  if( (sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
  {
    printf( "create socket error: %s( error:%d )\n", strerror( errno ), errno );
    exit(0);
  }

  //初始化服务器IP地址和端口号等信息
  memset( &serv_addr, 0, sizeof( serv_addr ) );
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons( SERVER_PORT );
  serv_addr.sin_addr.s_addr = htonl( INADDR_ANY );

  //将套接字与服务器地址绑定
  if( blind( sockfd, (struct sockaddr *)&serv_addr, sizeof( serv_addr ) ) < 0 )
  {
    printf(" blind scoket error: %s ( errno: %d )", strerror( errno ), errno );
    exit(0);
  }
	
  //监听套接字
  if( listen( sockfd, 10 ) < 0 )
  {
    printf(" listen socket error: %s\n", );
  }
  close( sockfd );
  return 0;
}
