#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<memory.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

#define SERVER_PORT 8000
#define BUFFER_MAX 4096
#define NUL '\0'

int main(int argc, char const *argv[]) {
  int sockfd, connectfd, recv_len;
  char send_buffer[BUFFER_MAX], recv_buffer[BUFFER_MAX];
  struct iovec io;
  struct sockaddr_in serv_addr;
  struct msghdr send_msg, recv_msg;

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
  if( bind( sockfd, (struct sockaddr *)&serv_addr, sizeof( serv_addr ) ) < 0 )
  {
    printf(" blind scoket error: %s ( errno: %d )", strerror( errno ), errno );
    exit(0);
  }

  //监听套接字
  if( listen( sockfd, 10 ) < 0 )
  {
    printf(" listen socket error: %s( errno: %d )\n", strerror( errno ), errno );
    exit(0);
  }

  printf("================waiting for client's request========================\n");

  while( 1 )
  {
    //阻塞直到有客户端连接，不然多浪费CPU资源。
    if( ( connectfd = accept( sockfd, (struct sockaddr *)NULL, NULL ) ) < 0)
    {
      printf(" accpet sockfd error: %s ( errno: %d )", strerror( errno ), errno );
      continue;
    }
    //初始化接受信息
    recv_msg.msg_name = NULL;
    io.iov_base = recv_buffer;
    io.iov_len = BUFFER_MAX;
    recv_msg.msg_iov = &io;
    recv_msg.msg_iovlen = 1;
    //接受客户端传来的数据
    recv_len = recvmsg( connectfd, &recv_msg, 0 );
    if( !fork() )
    {
      char num[100];
      strcat(send_buffer, "server have receive client ");
      //itoa( recv_len, num, 10);// Unix版本：itoa()在头文件<stdlib.h>中不存在
      sprintf( num, "%d", recv_len );
      strcat( send_buffer, num );
      strcat( send_buffer, recv_len > 1? " bytes.":" byte." );
      //printf("%s", send_buffer );
      send_msg.msg_name = NULL;
      io.iov_base = send_buffer;
      io.iov_len = BUFFER_MAX;
      send_msg.msg_iov = &io;
      send_msg.msg_iovlen = 1;

      if( sendmsg( connectfd, &send_msg, 0) < 0 )
        perror( "send error ");
      close( connectfd );
      exit(0);
    }
      recv_buffer[recv_len] = NUL;
      printf(" server receive from client message:\n %s", recv_buffer );
      close( connectfd );
  }
  close( sockfd );
  return 0;
}
