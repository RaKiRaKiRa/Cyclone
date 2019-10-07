/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-16 21:45
 * Last modified : 2019-10-07 20:31
 * Filename      : Connection.h
 * Description   : 
 **********************************************************/

#ifndef CONNECTION_H
#define CONNECTION_H

#include <memory>
#include "base/noncopyable.h"
#include "Socket.h"
#include "Callback.h"
#include "Buffer.h"

class EventLoop;
class Channel;

typedef std::shared_ptr<Connection> ConnectionPtr;

//表示一个TCP连接，并封装其基本行为,使用Channel来获得socket上的IO事件,自己处理writable事件,将readable事件通过MessageCallback传递给客户
//集成了enable_shared_from_this,可以安全地生成其他额外的 std::shared_ptr 实例
//调用已经被智能指针t指向的实例的shared_from_this 成员函数，将会返回一个新的 std::shared_ptr<T> 对象
class Connection : noncopyable, public std::enable_shared_from_this<Connection>
{
public:
  typedef std::function<void(const ConnectionPtr&)> ConnCallback;
  typedef std::function<void (const ConnectionPtr&)> CloseCallback;
  Connection(EventLoop *loop, std::string name, sockaddr_in local, sockaddr_in peer, int sockfd);//虽然有local地址和peer地址，但通信靠sockfd绑定
  ~Connection();

  //*************************************设置回调函数接口***************
  void setConnCallback(ConnCallback cb)
  {
    connCallback_ = std::move(cb);
  }
  void setMessCallback(MessCallback cb)
  {
    messCallback_ = std::move(cb);
  }
  void setCloseCallback(CloseCallback cb)
  {
    closeCallback_ = std::move(cb);
  }

  //************************************连接开关*************************
  void connEstablish();     //开启Channel的可读监听并调用connCallback
  void connDestroy();       //关闭Channel的监听并从poll移除,调用connCallback,在handleClose的下一次poll后进行

  //************************************获取信息
  std::string name() const
  {
    return name_;
  }
  std::string state() const
  {
    switch(state_)
    {
      case kConnected:    return "kConnected";break;
      case kConnecting:   return "kConnecting";break;
      case kDisconnected: return "kDisconnected";break;
      case kDisconnecting:return "kDisconnecting";break;
    }
    return "unknown state"; 
  }

  bool connected() const
  {
    return state_ == kConnected;
  }

  sockaddr_in peer() const
  {
    return peer_;
  }

  sockaddr_in local() const
  {
    return local_;
  }  

  EventLoop* loop() const
  {
    return loop_;
  }
  //TODO************************************连接关闭*******************************/
  void shutdown();
  void shutdownInLoop();
  //void forceClose(double seconds);  
  void forceClose();
  void delayClose(double seconds);
  void forceCloseInLoop();

  //TODO************************************连接选项控制**************************/
  void setNoDelay(bool on) 
  {
    socket_ -> setNodelay(on);
  }

  void setKeepAlive(bool on)
  {
    socket_ -> setKeepAlive(on);
  }

  //****************************************发送信息******************************/
  void send(const std::string& message);
  //void send(const char* data, size_t len);
  void send(Buffer* buf);
  //void sendInLoop(const char* data, size_t len);
  void sendInLoop(const std::string& message);


  void setAnyPtr(void* ptr)
  {
    anyPtr = ptr;
  }

  void *getAnyPtr() const
  {
    return anyPtr;
  }

  void setEntryPtr(void* ptr)
  {
    entryPtr = ptr;
  }

  void *getEntryPtr() const
  {
    return entryPtr;
  }
private:
  enum StateE {kConnecting, kConnected, kDisconnecting, kDisconnected};//连接状态
  void setState(StateE s)
  {
    state_ = s;
  }

  //监听端口事件触发后调用，在构造函数中绑定给channel_
  void handleRead();   //端口可读,并根据read返回值调用messageCallback_, handleClose, handleError
  void handleWrite();  //端口可写
  void handleClose();  //端口关闭, 调用closeCallback_=>Server::removeConnection,这里很重要详细看笔记+实现注释
  void handleError();  //端口出错, 仅日志输出

  EventLoop* loop_;
  std::string name_;
  StateE state_;
  sockaddr_in peer_;                  //对端地址
  sockaddr_in local_;                 //本地地址
  std::unique_ptr<Socket> socket_;    //用于连接控制的TCP套接字
  std::unique_ptr<Channel> channel_;  //及其对应Channel

  // 连接建立事件回调函数
  ConnCallback connCallback_;         
  // 消息接收事件回调函数（可读事件）
  MessCallback messCallback_;         
  /* 写入tcp缓冲区之后的回调函数
   * 通常是tcp缓冲区满然后添加到应用层缓冲区后，
   * 由应用层缓冲区写入内核tcp缓冲区后执行，一般用户不关系这部分
   */
  WriteCompleteCallback writeCompleteCallback_; 

  // 重点：
  // 关闭连接回调函数，绑定Server::removeConn()，从connection_移除本连接，引用计数-1，queueInLoop绑定connDestroy并传递conn，使引用计数+1
  // 由提供给Channel的Connection::handleClose()调用
  // 其执行完毕后下一个poll中调用Connction::ConnDestroy，移除loop中的对应Channel，结束后引用计数-1，销毁Connection.
  CloseCallback closeCallback_;       


  Buffer inputBuffer_;
  Buffer outputBuffer_;

  void* anyPtr;
  void* entryPtr;
};
  
#endif
