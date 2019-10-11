/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-12 16:13
 * Last modified : 2019-07-16 19:49
 * Filename      : Acceptor.h
 * Description   : 
 **********************************************************/
#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "Socket.h"
#include "EventLoop.h"
#include "Channel.h"

//将Socket与Channel再次封装成用于监听、接收新连接的RAII类
class Acceptor : noncopyable
{
public:
  typedef std::function<void(int sockfd, sockaddr_in& addr)> NewConnCallback;
  Acceptor(EventLoop *loop, const sockaddr_in& addr, bool reuseport = true);

  //析构将channel_从EventLoop和IPoller中移除
  //Socket本身是RAII的，生命周期与Acceptor相同不用管
  ~Acceptor();

  void listen();
  void setNewConnCallback(NewConnCallback cb)
  {
    newConnCallback_ = std::move(cb);
  }

  bool listening() const
  {
    return listening_;
  }

private:
  // 给channel的ReadCallback进行绑定，对于异步socket在listen后通过(e)poll发生可读事件即为完成三次握手，可以进行accept
  void handleRead();

  EventLoop *loop_;
  Socket socket_;
  Channel channel_;
  NewConnCallback newConnCallback_;//建立连接后的回调，用与建立TcpConnection,客户端套接字和地址作为参数传入，一般用于初始化一个Connection
  bool listening_;
};

#endif
