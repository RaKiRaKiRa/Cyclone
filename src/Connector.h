/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-06 23:15
 * Last modified : 2019-10-07 00:54
 * Filename      : Connector.h
 * Description   : 
 **********************************************************/
#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "Socket.h"
#include "base/noncopyable.h"
#include "SocketOpts.h"
#include "base/Logging.h"
#include <functional>
#include <memory>

class Eventloop;
class Channel;
// 对应Acceptor，主动发起连接，发起的连接用Connection封装，供Client使用
class Connector: noncopyable, std::enable_shared_from_this<Connector>
{
public:
  typedef std::function<void(int)> NewConnCallback;
  Connector(Eventloop* loop, sockaddr_in& addr);

  ~Connector();

  void setNewConnCallback(NewConnCallback cb)
  {
    newConnCallback_ = std::move(cb);
  }

  void start();
  void stop();

private:
  enum State
  {
    kDisconnected, 
    kConnecting, 
    kConnected
  };

  void setState(State state)
  {
    state_ = state;
  }

  State state() const
  {
    return state_;
  }

  void startInLoop();
  void stopInLoop();
  void connect();
  void connectComplete(int sockfd);
  void handleWrite();
  void handleError();
  void removeAndResetChannel();// 不能直接reset，需加入runInLoop,在下一次loop中调用，因为正处于Channel::handleEvent
  void resetChannel();


  Eventloop* loop_;
  sockaddr_in& serverAddr_;
  bool started_;
  State state_;
  std::unique_ptr<Channel> channel_;
  NewConnCallback newConnCallback_; // 用于建立连接后创造Connection
};

#endif
