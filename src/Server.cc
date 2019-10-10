/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-17 19:41
 * Last modified : 2019-10-09 23:53
 * Filename      : Server.cc
 * Description   : 
 **********************************************************/

#include "Server.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "Socket.h"
#include "EventLoopThreadPool.h"


Server::Server(EventLoop* loop,const sockaddr_in& listenAddr, const std::string& name, bool ReusePort):
  loop_(loop),
  name_(name),
  ipPort_(toIpPort(&listenAddr)),
  acceptor_(new Acceptor(loop, listenAddr, ReusePort)),
  threadPool_(new EventLoopThreadPool(loop_, name_)),
  started_(false),
  nextConnId_(1),
  connCallback_(defaultConnCallback),
  messCallback_(defaultMessCallback)
{
  acceptor_ -> setNewConnCallback(std::bind(&Server::newConn, this, _1, _2));
}

Server::~Server()
{
  loop_ -> assertInLoopThread();
  LOG_TRACE << "Server::~Server [" << name_ <<"] destructing" ;
  // 停止每个连接
  for(auto& connByName : connections_)
  {
    ConnectionPtr conn(connByName.second);
    // 使map中的ConnectionPtr移除，此时连接的引用仅为1
    connByName.second.reset();
    // 延长Connection的生命周期至其自己调用connDestroy 移除Channel
    conn->loop() -> runInLoop( std::bind(&Connection::connDestroy,conn) );
    // 然后线程调用join，至Connection生命周期结束后关闭线程
  }
}


void Server::setThreadNum(int num)
{
  threadPool_ -> setThreadNum(num);
}

void Server::start()
{
  if(!started_)
  {
    threadPool_ -> start();
    if(!acceptor_ -> listening())
      loop_ -> runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
  }

}


// 根据新连接的fd与addr建立Connection
/* 1.Server内有一个EventLoopThreadPool，即事件循环线程池，池子中每个线程都是一个EventLoop
 * 2.每个EventLoop包含一个Poller用于监听注册到这个EventLoop上的所有Channel
 * 3.当建立起一个新的Connection时，这个连接会放到线程池中的某个EventLoop中
 * 4.Server中的baseLoop只用来处理客户端的连接
 * 5.网络IO和计算任务都在workloop中进行
 */
 void Server::newConn(int sockfd, const sockaddr_in& peerAddr)
{
  loop_ -> assertInLoopThread();
  char buf[64];
  snprintf(buf, sizeof buf, "-%s#%d",ipPort_.c_str(), nextConnId_);
  ++nextConnId_;
  std::string connName = name_ + buf;//新连接的名字 server名-监听IP:PORT#连接序号

  LOG_INFO << "Server::newConn[" << name_ << "] - new connction [" << connName <<"] from " << toIpPort(&peerAddr); 

  // 当threadNum为0时，返回baseloop_即单线程模式
  EventLoop* ioLoop = threadPool_ -> nextLoop();
  //创建出是shared_ptr，引用喜加一
  ConnectionPtr conn(new Connection(ioLoop, connName, getLocalAddr(sockfd), peerAddr, sockfd));//虽然conn有local地址和peer地址，但通信还是靠sockfd完成
  connections_[connName] = conn;

  //设置连接回调函数并启动监听
  conn->setConnCallback(connCallback_);
  conn->setMessCallback(messCallback_);
  // removeConn作用是将连接从map中删除，由handleClose调用，再下一轮poll后才移除channel（目的是延长生命周期
  conn->setCloseCallback(std::bind(&Server::removeConn, this, _1));//_1由handleClose调用closeCallback_时传入自身，后传给Connection::connDestroy

  ioLoop -> runInLoop(std::bind(&Connection::connEstablish, conn));//开启对应Channel可读监听并调用connCallback
}

void Server::removeConn(const ConnectionPtr& conn)
{
  loop_ -> runInLoop(std::bind(&Server::removeConnInLoop, this, conn));
}

//绑定给Connection::CloseCallback,从connection_中移除对应conn，然后在下次poll后在loop_中运行Connection::connDestroy()移除channel
void Server::removeConnInLoop(const ConnectionPtr& conn)
{
  loop_ -> assertInLoopThread();
  LOG_INFO << "Server::removeConn [" << name_ << "] - connection " << conn -> name();
  //删除后引用计数-1，仅有本函数所引用的一个引用，
  //若不增加引用计数延长生命周期,则在Connection::handleClose()结束就会被销毁，
  //这会导致更外层的Connection::handleEvent()出错
  connections_.erase(conn->name());

  //为了延长生命周期，将loop删除Channel操作延长至下次poll后,所以一定要用queueInLoop而不是runInLoop(会直接执行)
  EventLoop* ioLoop = conn -> loop();
  /*
   * std::bind绑定函数指针，注意是值绑定，也就是说conn会复制一份到bind上
   * 这就会延长TcpConnection生命期，否则
   * 1.此时对于TcpConnection的引用计数为2，参数一个，connections_中一个
   * 2.connections_删除掉TcpConnection后，引用计数为1
   * 3.removeConnectionInLoop返回，上层函数handleClose返回，引用计数为0，会被析构
   * 4.bind会值绑定，conn复制一份，TcpConnection引用计数加1，就不会导致TcpConnection被析构
   */
  ioLoop -> queueInLoop(std::bind(&Connection::connDestroy, conn));
}
