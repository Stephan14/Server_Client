#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<memory.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define DEFAULT_PORT 1234
#define MAXLINE 1024
#define CONNECTION_MAX 10

struct context{
  char str[1024];
};

int main(int argc, char** argv)
{
    int    socket_fd, connect_fd;
    struct sockaddr_in servaddr, clientaddr;
    struct timeval timeout = { 3, 0 };
    struct context text;
    char    buff[ MAXLINE ], recvline[ MAXLINE ], sendline[ MAXLINE ];
    int     n, state, connection_mount = 0, fd_active[ CONNECTION_MAX ];
    int clientaddr_size = sizeof( clientaddr );
    fd_set file_des_set;//无struct

    memset( fd_active, 0, CONNECTION_MAX );
    //初始化Socket
    if( ( socket_fd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
    {
      printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
      exit(0);
    }
    //初始化服务器地址
    memset( &servaddr, 0, sizeof( servaddr ) );
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl( INADDR_ANY );//IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址。
    servaddr.sin_port = htons( DEFAULT_PORT );//设置的端口为DEFAULT_PORT

    //将本地地址绑定到所创建的套接字上
    if( bind( socket_fd, (struct sockaddr*)&servaddr, sizeof( servaddr ) ) == -1)
    {
      printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
      exit(0);
    }
    //开始监听是否有客户端连接
    if( listen( socket_fd, CONNECTION_MAX ) == -1 )
    {
      printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
      exit(0);
    }
    printf("======waiting for client's request======\n");
    int maxsock = socket_fd;

    while(1){
        //每一次循环都清空集合，否则不能够检查到文件标识符是否变化
        FD_ZERO( &file_des_set );
        //添加文件描述符
        FD_SET( socket_fd, &file_des_set );
        timeout.tv_sec = 30;
        timeout.tv_usec = 0;
        //向文件标识符集合中添加活跃的连接,fd_active数组中不包含欢迎套接字，否则会是整个进程阻塞
        int index;
        for( index = 0; index < CONNECTION_MAX; index++ )
          if( fd_active[ index ] != 0 )
            FD_SET( fd_active[ index ], &file_des_set );

        state = select( maxsock + 1, &file_des_set, NULL, NULL, &timeout );
        if( state < 0 )
        {
          printf("select file descripter set is null! \n");
          break;//break;
        }
        else if( state == 0 )
        {
          printf( "select timeout!\n" );
          continue;
        }
        //检查文件字符集合中的文件标识符
        for( index = 0; index < connection_mount; index++ )
        {
            if( FD_ISSET( fd_active[ index ], &file_des_set ) )
            {
              //接受客户端传过来的数据
              if( ( n = recv( fd_active[ index ], buff, MAXLINE, 0  ) ) <= 0)
              { //文件描述符集合中的套接字没有收到客户端传送的数据，则关闭连接
                printf("client %d closed!\n", index );
                close( fd_active[ index ] );
                FD_CLR( fd_active[ index ], &file_des_set );
                fd_active[ index ] = 0;
                connection_mount--;
              }
              else
              {
                if( n <= MAXLINE )
                  strcpy( recvline, ( (struct context *)buff )->str );
                printf("recv msg from client[%d]: %s\n", index + 1, recvline );
                //向客户端发送回应数据
                sprintf( text.str, "server have receive %d bytes!\n", strlen( recvline ) );
                if( send( fd_active[ index ], ( char * )&text, sizeof( struct context ), 0 ) == -1)
                  perror("send error");
              }
            }
        }
        //检查是否产生一个新的连接
        if( FD_ISSET( socket_fd, &file_des_set ) )
        {
          //阻塞直到有客户端连接，不然多浪费CPU资源
          if( ( connect_fd = accept( socket_fd, (struct sockaddr*)&clientaddr, &clientaddr_size ) ) == -1)
          {
            printf("accept socket error: %s(errno: %d)\n",strerror(errno),errno);
            continue;
          }
          //将新建立的套接字添加到文件标识符集合中
          if( connection_mount < CONNECTION_MAX )
          {
            fd_active[ connection_mount++ ] = connect_fd;
            printf("new connection client[%d] %s:%d \n", connection_mount, inet_ntoa( clientaddr.sin_addr), ntohs( clientaddr.sin_port ) );
            if( connect_fd > maxsock )
              maxsock = connect_fd;
          }
          else
          {
            printf("max connection arrive, exit \n" );
            strcpy( text.str, "bye" );
            send( connect_fd, (char *)&text, sizeof( struct context ), 0 );
            close( connect_fd );
            break;
          }

        }

        // for( index = 0; index < connection_mount; index++ )
        // {
        //   if( fd_active[ index ] != 0 )
        //     close( fd_active[ index ] );
        // }
    }
    close(socket_fd);
}
