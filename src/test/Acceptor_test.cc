/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-13 16:13
 * Last modified : 2019-07-13 16:25
 * Filename      : Acceptor_test.cc
 * Description   : 
 **********************************************************/


#include "../Acceptor.h"
#include "../EventLoop.h"
#include <unistd.h>

void newconn(int sockfd, const sockaddr_in& addr)
{
  char buf[1024];
  toIpPort(buf,1024, &addr);
  printf("newConn(): accepted a new conn from %s \n", buf);
  write(sockfd, "How Are You?\n", 13);
  Close(sockfd);
}

int main()
{
  printf("main(): pid = %d\n", getpid());

  printf("0");
  sockaddr_in addr;
  printf("0");
  fromPort(9981, &addr);
  printf("0");
  EventLoop loop;
  printf("1");
  Acceptor acceptor(&loop, addr);
  printf("2");
  acceptor.setNewConnCallback(newconn);
  printf("3");
  acceptor.listen();
  printf("4");

  loop.loop();
}
