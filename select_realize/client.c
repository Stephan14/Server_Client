#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

#define MAXLINE 1024
#define DEFAULT_PORT 1234
struct context{
  char str[1024];
};
int main(int argc, char** argv)
{
    int    sockfd, n,rec_len;
    char    recvline[1024], sendline[1024];
    char    buf[MAXLINE];
    struct sockaddr_in    servaddr;
    struct context text;

    if( argc != 2)
    {
      printf("usage: ./client <ipaddress>\n");
      exit(0);
    }

    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0)
    {
      printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
      exit(0);
    }

    memset(&servaddr, 0, sizeof( servaddr ) );
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons( DEFAULT_PORT );
    if( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0 )
    {
      printf("inet_pton error for %s\n",argv[1]);
      exit(0);
    }

    if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr) ) < 0)
    {
      printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
      exit(0);
    }

    while( 1 )
    {
      printf("send msg to server：\n");
      fgets(sendline, 1024, stdin);
      if( strcmp( sendline, "exit") == 0 )
        break;
      strcpy( text.str, sendline );
      if( send(sockfd, (char *)&text, sizeof( struct context ), 0 ) < 0 )
      {
        printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
      }
      if( ( rec_len = recv(sockfd, buf, MAXLINE,0 ) ) == -1 )
      {
         perror("recv error");
         exit(1);
      }
      strcpy( recvline, ( ( struct context *)buf )->str );
      printf("recv msg from server：\n%s", recvline );
    }
    close(sockfd);
    return 0;
}
