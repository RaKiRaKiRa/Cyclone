/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-17 01:02
 * Last modified : 2019-07-17 22:07
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

/*
 * 要想使用Server，需要
 *
 *  创建一个事件驱动循环，也就是Server所在线程的EventLoop（用于监听客户端连接请求）
 *  创建监听地址和端口
 *  创建Server并传入EventLoop和监听地址listenAddr
 *  设置各种回调函数
 *  调用Server::start()开启服务器
 *    调用EventLoop::loop()开始事件驱动循环
 */

class Server : noncopyable
{
public:
  typedef std::function<void (const ConnectionPtr&)> ConnCallback;          //连接事件
  typedef std::function<void (const ConnectionPtr&, Buffer*)> MessCallback; //消息到达事件，fd可读
  typedef std::function<void (const ConnectionPtr&)> WriteCompleteCallback; // 数据已从应用层写入缓冲区，并不代表对方已收到
  typedef std::shared_ptr<Connection> ConnectionPtr;

  Server(EventLoop* loop, sockaddr_in& listenAddr);
  ~Server();

  void start();

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

private:
  typedef std::map<std::string, ConnectionPtr> ConnectionMap;

  //创建新的Connection，绑定给Acceptor，新连接将加入ConnectionMap
  void newConn(int sockfd, const sockaddr_in& peerAddr);

  EventLoop* loop_;
  const std::string name_; 
  const std::string ipPort_;              //toIpPort(&peerAddr).c_str(),
  std::unique_ptr<Acceptor> acceptor_;    //用于接收新连接的接收器
  ConnectionMap connections_;              //name->Connection*映射

  bool started_;
  int nextConnId_;//下个连接的索引

  // 供用户设置的回调函数
  ConnCallback connCallback_;                   // 连接建立/断开事件回调函数
  MessCallback messCallback_;                   // 收到消息时的回调函数，一般是读取并操作
  WriteCompleteCallback writeCompleteCallback_; // 数据已从应用层写入缓冲区，并不代表对方已收到


};

#endif
