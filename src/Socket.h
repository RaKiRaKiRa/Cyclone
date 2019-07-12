/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-09 22:26
 * Last modified : 2019-07-12 16:48
 * Filename      : Socket.h
 * Description   : 
 **********************************************************/

#ifndef SOCKET_H
#define SOCKET_H

#include "base/noncopyable.h"
#include "SocketOpts.h"

// RAII 封装socket api
class Socket : noncopyable
{
public:
  Socket() :
    sockfd_(createNonblockingSockfd())
  {}

  explicit Socket(int sockfd):
    sockfd_(sockfd)
  {}

  ~Socket()
  {
    Close(sockfd_);
  }

  int fd() const
  {
    return sockfd_;
  }

/******************通信api************************/
  void bind(const struct sockaddr_in* addr);

  void listen();

  int accept(sockaddr_in* addr);

  //仅关闭写，可以接收完数据
  void shutdownWrite();

/******************设置api************************/
  //非阻塞
  void setNonblock(bool on);

  //地址复用
  void setReuseAddr(bool on);

  //端口复用
  void setReusePort(bool on);
  
  //Nagle
  void setNodelay(bool on);

  //TCP层心跳
  void setKeepAlive(bool on);

private:
  const int sockfd_;
};

#endif
