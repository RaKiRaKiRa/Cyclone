/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-10 21:37
 * Last modified : 2019-07-10 23:12
 * Filename      : Socket.cc
 * Description   : 
 **********************************************************/

#include "Socket.h"
#include <memory.h>

void Socket::bind(const struct sockaddr_in* addr)
{
  Bind(sockfd_, addr);
}

void Socket::listen()
{
  Listen(sockfd_);
}

int Socket::accept(sockaddr_in* addr)
{
  memset(addr, 0, sizeof(*addr));
  int connfd = Accept(sockfd_, addr);
  return connfd;
}

void Socket::shutdownWrite()
{
  ShutdownWrite(sockfd_);
}

void Socket::setNonblock(bool on)
{
  ::setNonblock(sockfd_, on);
}

void Socket::setReuseAddr(bool on)
{
  ::setReuseAddr(sockfd_, on);
}

void Socket::setReusePort(bool on)
{
  ::setReusePort(sockfd_, on);
}
