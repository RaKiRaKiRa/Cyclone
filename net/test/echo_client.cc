#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/tcp.h>

int main()
{
  sockaddr_in addr;
  bzero(&addr, sizeof addr);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(4567);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(connect(sockfd, (sockaddr*)&addr, sizeof addr) < 0)
  {
    close(sockfd);
    printf("error");
    exit(-1);
  }
  else
  {
    printf("ok\n");
  }
  char buf[128];
  while(1)
  {
    bzero(buf,sizeof(buf));
    scanf("%s",buf);
    if(strcmp(buf,":quit") == 0)
      break;
    struct tcp_info info;
    int len=sizeof(info);
    getsockopt(sockfd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
    if((info.tcpi_state!=TCP_ESTABLISHED))
      break;
    write(sockfd,buf,sizeof(buf));//写数据
    bzero(buf,sizeof(buf));
    read(sockfd,buf,sizeof(buf));//读数据
    printf("echo:%s\n",buf);
  }
  shutdown(sockfd,SHUT_WR);
 // close(sockfd);
  return 0;
}
