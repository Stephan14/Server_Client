#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<memory.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<sys/time.h>

#define DEFAULT_SERVER_PORT 1234
#define CONNECTION_MAX 10
#define BUFFER_MAX 1024

int main(int argc, char  *argv[])
{
  int welcome_sock_fd, conn_sock_fd;
  int max_index = 0;
  int active_sock_set[ CONNECTION_MAX ] = { 0 };
  char recv_buffer[ BUFFER_MAX ], send_buffer[ BUFFER_MAX ];
  struct sockaddr_in serveraddr, clientaddr;
  struct timeval timeout = { 3, 0 };
  fd_set sock_set, all_set;
  memset( active_sock_set, 0, sizeof( active_sock_set ) );
  if( ( welcome_sock_fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
  {
    printf("socket error : %s \n", strerror( errno ) );
    return -1;
  }

  memset( &serveraddr, 0, sizeof( struct sockaddr_in ) );
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl( INADDR_ANY );
  serveraddr.sin_port = htons( DEFAULT_SERVER_PORT );

  if( bind( welcome_sock_fd, (struct sockaddr * )&serveraddr, sizeof( struct sockaddr_in ) ) < 0 )
  {
    printf("bind error : %s \n", strerror( errno ) );
    return -1;
  }

  if( listen( welcome_sock_fd, CONNECTION_MAX ) < 0 )
  {
    printf("listen error : %s \n", strerror( errno ) );
    return -1;
  }

  int max_sock = welcome_sock_fd;
  FD_ZERO( &sock_set );
  FD_SET( welcome_sock_fd, &sock_set );

  while( 1 )
  {
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    all_set = sock_set;

    int state = select( max_sock + 1, &all_set, NULL, NULL, &timeout );
    if( state < 0 )
    {
      printf("select file descripter set is null \n");
      break;
    }
    else if( state == 0 )
    {
      printf("timeout\n");
      continue;
    }
    int index;
    if( FD_ISSET( welcome_sock_fd, &all_set ) )
    {
      int clientaddr_size = sizeof( clientaddr );
      conn_sock_fd = accept( welcome_sock_fd, ( struct sockaddr *)& clientaddr, &clientaddr_size );
      for( index = 0; index < FD_SETSIZE; index++ )
        if( active_sock_set[ index ] == 0 )
        {
          active_sock_set[ index ] = conn_sock_fd;
          printf("new connection %d create!\n", active_sock_set[ index ] );
          break;
        }

      if( index == FD_SETSIZE )
        printf("too many clients !\n");
      FD_SET( conn_sock_fd , &sock_set );
      if( index > max_index )
        max_index = index;
      if( conn_sock_fd > max_sock )
        max_sock = conn_sock_fd;
      printf(" state : %d\n", state );
      if( --state <= 0 )
        continue;
    }

    for( index = 0; index < max_index + 1; index++ )
    {
      if( active_sock_set[ index ] == 0 )
        continue;

      if( FD_ISSET( active_sock_set[ index ], &all_set ) )
      {
        int n;
        if( ( n = recv( active_sock_set[ index ], recv_buffer, BUFFER_MAX, 0 ) ) <= 0 )
        {
          printf("client %d closed !\n", active_sock_set[ index ] );
          close( active_sock_set[ index ] );
          FD_CLR( active_sock_set[ index ], &sock_set );
          active_sock_set[ index ] = 0;
        }
        else
        {
          if( n < BUFFER_MAX )
            recv_buffer[ n ] = '\0';
          printf("receive from client %d : %s\n", active_sock_set[ index ], recv_buffer );
          sprintf(send_buffer, "server receive %d bytes!\n", n );
          if( send( active_sock_set[ index ], send_buffer, strlen( send_buffer ), 0 ) <= 0 )
            printf("send error : %s \n", strerror( errno ) );
        }
        if( --state <= 0 )
          break;
      }
    }
  }
  
  close( welcome_sock_fd );
  return 0;
}
