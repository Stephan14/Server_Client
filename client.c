#include<stdio.h>
#include<stdlib.h>//使用exit(0)
#include<errno.h>//使用errno、
#include<string.h>//使用strerror()函数
#include<memory.h>//使用memset()函数
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>

#define BUFFER_MAX 4096
#define NUL '\0'

int main(int argc, char const *argv[]) {
  int sockfd, recv_len;
  char recv_buffer[BUFFER_MAX], send_buffer[BUFFER_MAX];
  struct sockaddr_in serv_addr;
  struct iovec io;
  struct msghdr send_msg, recv_msg;

  if( argc != 2)
  {
    printf("usage: ./client <ipaddress>\n");
    exit(0);
  }
  //创建socket套接字
  if( sockfd = ( socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
  {
    printf( "create socket error: %s( error:%d )\n", strerror( errno ), errno );
    exit(0);
  }

  //设置服务器的IP地址和端口号等信息
  memset( &serv_addr, 0, sizeof( serv_addr ) );
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons( 8000 );//htons用来将主机字节顺序转化为网络字节顺序
  if( inet_pton( AF_INET, argv[1], &serv_addr.sin_addr ) < 0 )//将“点分十进制” －> “二进制整数”
  {
    printf("inte_pton error for %s\n", argv[1] );
    exit(0);
  }
  //连接服务器
  if( connect( sockfd, (struct sockaddr* )&serv_addr, sizeof(serv_addr) ) < 0 )
  {
    printf(" connect error: %s( errno: %d )\n", strerror( errno ), errno );
    exit(0);
  }
  //初始化发送信息
  printf("send message to server:\n");
  fgets( send_buffer, BUFFER_MAX, stdin );
  send_msg.msg_name = NULL;
  io.iov_base = send_buffer;
  io.iov_len = strlen( send_buffer );
  send_msg.msg_iov = &io;
  send_msg.msg_iovlen = 1;
  //向服务器发送信息
  if( sendmsg( sockfd, &send_msg, 0 ) < 0 )
  {
    printf("send message to message error : %s( errno: %d)", strerror( errno ), errno );
    exit(0);
  }

  //初始化接受信息
  recv_msg.msg_name = NULL;
  io.iov_base = recv_buffer;
  io.iov_len = BUFFER_MAX;
  recv_msg.msg_iov = &io;
  recv_msg.msg_iovlen = 1;

  //接收服务器发送的信息
  if( (recv_len = recvmsg( sockfd, &recv_msg, 0 ) ) < 0 )
  {
    perror( "recv error ");
    exit(1);
  }
  recv_buffer[recv_len] = NUL;
  printf( "receive message from server :%s", recv_buffer );

  //关闭连接
  close( sockfd );

  return 0;
}
