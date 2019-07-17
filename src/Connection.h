/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-16 21:45
 * Last modified : 2019-07-18 00:58
 * Filename      : Connection.h
 * Description   : 
 **********************************************************/

#ifndef CONNECTION_H
#define CONNECTION_H

#include <memory>
#include "base/noncopyable.h"
#include "Socket.h"
#include "Callback.h"

class EventLoop;
class Channel;

//表示一个TCP连接，并封装其基本行为,使用Channel来获得socket上的IO事件,自己处理writable事件,将readable事件通过MessageCallback传递给客户
//集成了enable_shared_from_this,可以安全地生成其他额外的 std::shared_ptr 实例
//调用已经被智能指针t指向的实例的shared_from_this 成员函数，将会返回一个新的 std::shared_ptr<T> 对象
//在异步调用中，存在一个保活机制，异步函数执行的时间点我们是无法确定的，
//然而异步函数可能会使用到异步调用之前就存在的变量。
//为了保证该变量在异步函数执期间一直有效，我们可以传递一个指向自身的share_ptr给异步函数，
//这样在异步函数执行期间share_ptr所管理的对象就不会析构，所使用的变量也会一直有效了
class Connection : noncopyable, public std::enable_shared_from_this<Connection>
{
public:
  typedef std::function<void(const ConnectionPtr&)> ConnCallback;
  typedef std::function<void()> CloseCallback;
  Connection(EventLoop *loop, std::string name, sockaddr_in local, sockaddr_in peer, int sockfd);//虽然有local地址和peer地址，但通信靠sockfd绑定
  ~Connection();

  //设置回调函数接口
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


  void connEstablished();   //开启Channel的可读监听并调用connCallback
private:
  enum StateE {kConnecting, kConnected, kDisconnected};//连接状态
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

  ConnCallback connCallback_;         //连接建立事件回调函数
  MessCallback messCallback_;
  CloseCallback closeCallback_;
};

#endif
