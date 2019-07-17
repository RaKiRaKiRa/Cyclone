/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-10 15:55
 * Last modified : 2019-07-18 01:01
 * Filename      : SocketOpts.cc
 * Description   : 
 **********************************************************/

#include "base/AsyncLogging.h"
#include "SocketOpts.h"
#include <cerrno>
#include <iostream>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <string.h>
  
//×××××××××××××××对socket进行封装，增加报错×××××××××××××

int createNonblockingSockfd()
{
  int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if(sockfd < 0)
  {
    LOG_ERROR << "ERROR :SocketOpts -- createNonblockingSockfd ";
   // std::cerr<< "ERROR :SocketOpts -- createNonblockingSockfd \n";
  }
  //创建完成后设置非阻塞
  setNonblock(sockfd, true);
  return sockfd;
}

int Connect(int sockfd, const struct sockaddr_in* addr)
{
  return connect(sockfd, sockaddr_cast(addr), static_cast<socklen_t>(sizeof(struct sockaddr_in)));
}

void Bind(int sockfd ,const struct sockaddr_in* addr)
{
  int ret = bind(sockfd, sockaddr_cast(addr), static_cast<socklen_t>(sizeof(struct sockaddr_in)));
  if(ret < 0)
  {
    LOG_ERROR << "ERROR : SocketOpts -- Connect ";
    //std::cerr << "ERROR : SocketOpts -- Connect \n";
  }
}

void Listen(int sockfd)
{
  int ret = listen(sockfd, SOMAXCONN);// SOMAXCONN是每一个端口最大的监听队列的长度,默认值为1024
  if(ret < 0)
  {
    LOG_ERROR << "ERROR : SocketOpts -- Listen ";
  }

}

int Accept(int sockfd, sockaddr_in* addr)
{
  socklen_t len = static_cast<socklen_t>(sizeof(*addr));
  int connfd = accept(sockfd, sockaddr_cast(addr), &len);
  //检验accept错误
  if(connfd < 0)
  {
    int errsaved = errno;
    switch(errsaved)
    {
      case EAGAIN:
      case ECONNABORTED:
      case EINTR:
      case EPROTO:
      case EPERM:
      case EMFILE:
        errno = errsaved;
        break;
      case EBADF:
      case EFAULT:
      case EINVAL:
      case ENFILE:
      case ENOBUFS:
      case ENOMEM:
      case ENOTSOCK:
      case EOPNOTSUPP:
        // unexpected errors
        LOG_FATAL << "unexpected error of ::accept " << errsaved;
        break;
      default:
        LOG_FATAL << "unknown error of ::accept " << errsaved;
        break;
    }
  }
  else
    setNonblock(connfd, true);
  return connfd;
}

void ShutdownWrite(int sockfd)
{
  if(shutdown(sockfd, SHUT_WR) < 0)
  {
    LOG_ERROR << "ERROR : SocketOpts -- ShutdownWrite ";
  }
}

void Close(int sockfd)
{
  if(close(sockfd) < 0)
  {
    LOG_ERROR << "ERROR : SocketOpts -- Close ";
  }
}
//×××××××××××××××对socket选项进行封装××××××××××××××××××××

void setNonblock(int sockfd, bool on)
{
  int flag = fcntl(sockfd, F_GETFL, 0);

  //检查错误
  if(flag < 0)
  {
    LOG_ERROR << "ERROR : SocketOpts -- setNonblock ";
    //printf("%s\n", strerror(errno));
    //std::cerr << "ERROR : SocketOpts -- setNonblock \n";

  }
  if(on)
    flag |= O_NONBLOCK;
  else if(flag & O_NONBLOCK)
    flag &= ~O_NONBLOCK;
  else
    return;
    
  //修改并检错
  if(fcntl(sockfd, F_SETFL, flag) < 0)
  {
    //std::cerr << "ERROR : SocketOpts -- setNonblock \n";
    LOG_ERROR << "ERROR : SocketOpts -- setNonblock ";
    printf("%s\n", strerror(errno));
  }
}

void setReusePort(int sockfd, bool on)
{
  int opt = on ? 1 : 0;
  int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof opt));
  if(on && ret < 0)
  {
    LOG_ERROR << "ReusePort failed. ";
  }
}


void setReuseAddr(int sockfd, bool on)
{
  int opt = on ? 1 : 0;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof opt));
}

//开关Nagle
void setNodelay(int sockfd, bool on)
{
  int opt = on ? 1 : 0;
  setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof opt));

}

//开关Tcp默认心跳
void setKeepAlive(int sockfd, bool on)
{
  int opt = on ? 1 : 0;
  setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &opt, static_cast<socklen_t>(sizeof opt));
}

void toIpPort(char* buf, size_t size, const struct sockaddr_in *addr)
{
  char host[16];//3*4 + 3 + 1
  inet_ntop(AF_INET, &addr->sin_addr, host, sizeof host);
  uint16_t port = networkToHost16(addr -> sin_port);
  snprintf(buf, size, "%s:%u", host, port);
}

std::string toIpPort(const struct sockaddr_in *addr)
{
  char buf[64];
  char host[16];//3*4 + 3 + 1
  inet_ntop(AF_INET, &addr->sin_addr, host, sizeof host);
  uint16_t port = networkToHost16(addr -> sin_port);
  snprintf(buf, 64, "%s:%u", host, port);
  std::string res(buf);
  return res;
}

void fromPort(uint16_t port, struct sockaddr_in *addr)
{
  memset(addr, 0, sizeof(*addr));
  addr -> sin_family = AF_INET;
  addr -> sin_port   = hostToNetwork16(port);
  addr -> sin_addr.s_addr = hostToNetwork32(INADDR_ANY);
}

void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in *addr)
{
  memset(addr, 0, sizeof(*addr));
  addr -> sin_family = AF_INET;
  addr -> sin_port   = hostToNetwork16(port);
  if(inet_pton(AF_INET , ip, &addr->sin_addr) < 0)
  {
    LOG_ERROR << "SocketOpts -- fromIpPort ";
  }
}

struct sockaddr_in getAddr(int sockfd)
{
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof addr);
  socklen_t len = static_cast<socklen_t>(sizeof addr);
  if(getpeername(sockfd, sockaddr_cast(&addr), &len) < 0)
  {
    LOG_ERROR << "SocketOpts -- getAddr";
  }
  return addr;
}

