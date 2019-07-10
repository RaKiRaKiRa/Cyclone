/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-10 15:55
 * Last modified : 2019-07-10 21:18
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
  
//×××××××××××××××对socket进行封装，增加报错×××××××××××××

int createNonblockingSockfd()
{
  int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if(sockfd < 0)
  {
    LOG_ERROR << "ERROR :SocketOpts -- createNonblockingSockfd \n";
   // std::cerr<< "ERROR :SocketOpts -- createNonblockingSockfd \n";
  }
  //创建完成后设置非阻塞
  setNonblock(sockfd, true);
  return sockfd;
}

int Connect(int sockfd, const struct sockaddr* addr)
{
  return connect(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in)));
}

void Bind(int sockfd ,struct sockaddr* addr)
{
  int ret = bind(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in)));
  if(ret < 0)
  {
    LOG_ERROR << "ERROR : SocketOpts -- Connect \n";
    //std::cerr << "ERROR : SocketOpts -- Connect \n";
  }
}

void Listen(int sockfd)
{
  int ret = listen(sockfd, SOMAXCONN);// SOMAXCONN是每一个端口最大的监听队列的长度,默认值为1024
  if(ret < 0)
  {
    LOG_ERROR << "ERROR : SocketOpts -- Listen \n";
  }

}

int Accept(int sockfd, sockaddr* addr)
{
  socklen_t len = static_cast<socklen_t>(sizeof(*addr));
  int connfd = accept(sockfd, addr, &len);
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

  setNonblock(connfd, true);
  return connfd;
}

void ShutdownWrite(int sockfd)
{
  if(shutdown(sockfd, SHUT_WR) < 0)
  {
    LOG_ERROR << "ERROR : SocketOpts -- ShutdownWrite \n";
  }
}
//×××××××××××××××对socket选项进行封装××××××××××××××××××××

void setNonblock(int sockfd, bool on)
{
  int flag = fcntl(sockfd, F_GETFL, 0);

  //检查错误
  if(flag < 0)
  {
    LOG_ERROR << "ERROR : SocketOpts -- setNonblock \n";
    //std::cerr << "ERROR : SocketOpts -- setNonblock \n";

  }

  flag |= O_NONBLOCK;
  //修改并检错
  if(fcntl(sockfd, F_SETFL, flag) < 0)
  {
    //std::cerr << "ERROR : SocketOpts -- setNonblock \n";
    LOG_ERROR << "ERROR : SocketOpts -- setNonblock \n";
  }
}

void setReusePort(int sockfd, bool on)
{
  int opt = on ? 1 : 0;
  int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof opt));
  if(on && ret < 0)
  {
    LOG_ERROR << "ReusePort failed. \n";
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

void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in *addr)
{
  addr -> sin_family = AF_INET;
  addr -> sin_port   = hostToNetwork16(port);
  if(inet_pton(AF_INET , ip, &addr->sin_addr))
  {
    LOG_ERROR << "SocketOpts -- fromIpPort \n";
  }
}
