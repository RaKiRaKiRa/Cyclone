/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-17 01:02
 * Last modified : 2019-10-08 21:54
 * Filename      : Server.h
 * Description   : 
 **********************************************************/

#ifndef SERVER_H
#define SERVER_H

#include "Connection.h"
#include "base/Atomic.h"
#include <map>

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

//Server运行监听并注册事件讲解：
/*
 * 1.创建服务器(Server)时，创建Acceptor，设置接收到客户端请求后执行的回调函数
 * 2.Acceptor创建监听套接字，将监听套接字绑定到一个Channel中，设置可读回调函数为Acceptor::handleRead
 * 3.服务器启动，调用Acceptor的listen函数创建监听套接字，同时将Channel添加到Poller中
 * 4.有客户端请求连接，监听套接字可读，Channel被激活，调用可读回调函数(handleRead)
 * 5.回调函数接收客户端请求，获得客户端套接字和地址，调用Server提供的回调函数(newConn)
 * 6.Server的回调函数中创建Connection代表这个tcp连接，设置tcp连接各种回调函数(由用户提供给Server的三个半)
 * 7.Server让tcp连接所属线程调用Connection::connectEstablish
 * 8.connectEstablish开启对客户端套接字的Channel的可读监听，然后调用用户提供的回调函数
 */ 

//用户可以自己提供三个半事件
//1.连接的建立 connCallback_
//2.连接的断开 connCallback_
//3.消息到达（描述符可读）messCallback_
//4.消息发送完毕 writeCallback_
class Server : noncopyable
{
public:
  typedef std::function<void (const ConnectionPtr&)> ConnCallback;          //连接事件
  typedef std::function<void (const ConnectionPtr&, Buffer*)> MessCallback; //消息到达事件，fd可读
  typedef std::function<void (const ConnectionPtr&)> WriteCompleteCallback; // 数据已从应用层写入缓冲区，并不代表对方已收到
  typedef std::shared_ptr<Connection> ConnectionPtr;

  Server(EventLoop* loop, const sockaddr_in& listenAddr, const std::string& name = "", bool ReusePort = true);

  ~Server();

  void start();

  void setThreadNum(int num);

  //设置回调
  void setConnCallback(ConnCallback cb)
  {
    connCallback_ = std::move(cb);
  }
  void setMessCallback(MessCallback cb)
  {
    messCallback_ = std::move(cb);
  }
  void setWriteCompleteCallback(WriteCompleteCallback cb)
  {
    writeCompleteCallback_ = std::move(cb);
  }

  std::string name() const
  {
    return name_;
  }

  std::string ipPort() const
  {
    return ipPort_;
  }

  EventLoop* loop() const
  {
    return loop_;
  }

private:
  typedef std::map<std::string, ConnectionPtr> ConnectionMap;

  //创建新的Connection，绑定给Acceptor，新连接将加入ConnectionMap
  // 执行调用顺序：poller_.poll()=>Channel::HandleEvent()=>Acceptor::HandleRead()=>Server::newConn()
  void newConn(int sockfd, const sockaddr_in& peerAddr);

  void removeConn(const ConnectionPtr&);
  void removeConnInLoop(const ConnectionPtr&);

  EventLoop* loop_;
  const std::string name_; 
  const std::string ipPort_;              //toIpPort(&peerAddr).c_str(),
  std::unique_ptr<Acceptor> acceptor_;    //用于接收新连接的接收器
  std::unique_ptr<EventLoopThreadPool> threadPool_;
  ConnectionMap connections_;              //name->Connection*映射

  bool started_;
  int nextConnId_;//下个连接的ID,用于命名

  // 供用户设置的回调函数
  ConnCallback connCallback_;                   // 连接建立/断开事件回调函数
  MessCallback messCallback_;                   // 收到消息时的回调函数，一般是读取并操作
  WriteCompleteCallback writeCompleteCallback_; // 数据已从应用层写入缓冲区，并不代表对方已收到


};

#endif
