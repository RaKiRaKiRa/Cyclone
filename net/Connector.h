/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-06 23:15
 * Last modified : 2019-10-17 01:37
 * Filename      : Connector.h
 * Description   : 
 **********************************************************/
#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "Socket.h"
#include "base/noncopyable.h"
#include "SocketOpts.h"
#include "Timer.h"
#include <functional>
#include <memory>

class EventLoop;
class Channel;
// 对应Acceptor，主动发起连接，发起的连接用Connection封装，供Client使用
// socket是一次性的，一旦出错无法恢复。仅仅能关闭重来。使用新的fd后，用新的channel。
class Connector: public noncopyable, public std::enable_shared_from_this<Connector>
{
public:
  typedef std::function<void (int)> NewConnCallback;
  
  Connector(EventLoop* loop, sockaddr_in& addr);
  ~Connector();

  void setNewConnCallback(NewConnCallback cb)
  {
    newConnCallback_ = std::move(cb);
  }

  //Connector::start()-->Connector::startInLoop()-->Connector::connect()-->Connector::connecting()
  //然后通过判断是否可写 调用handleWrite判断连接是否成功
  //若成功，则先removeAndResetChannel,然后newConnCallback_让Connection接管连接
  void start();  
  // 将正在连接改为断开连接
  // 若已连接，则无视
  // 断开连接操作由Client和Connection进行 
  void stop();    
  void restart(); // 将作为Connection的closeCallback,在非主动关闭时进行重连

  sockaddr_in serverAddr() const 
  {
    return serverAddr_;
  }  

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
  // 建立连接
  void connect();
  // 根据建立的连接生成Channel 判断连接是否可写,在handleWrite中继续下一步
  void connecting(int sockfd);
  void handleWrite();
  void handleError();
  int  removeAndResetChannel();// 不能直接reset，需加入runInLoop,在下一次loop中调用，因为正处于Channel::handleEvent
  void resetChannel();
  void retry(int sockfd);

  EventLoop* loop_;
  sockaddr_in& serverAddr_;
  bool started_;
  State state_;
  std::unique_ptr<Channel> channel_;
  NewConnCallback newConnCallback_; // 用于建立连接后创造Connection
  TimerId id_;
  int retryDelayMs_;

  static const int kMaxRetryDelayMs = 30*1000;
  static const int kInitRetryDelayMs = 500;
};

#endif
