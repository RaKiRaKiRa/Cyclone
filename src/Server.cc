/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-17 19:41
 * Last modified : 2019-07-18 01:00
 * Filename      : Server.cc
 * Description   : 
 **********************************************************/

#include "Server.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "Socket.h"

// 根据新连接的fd与addr建立Connection
void Server::newConn(int sockfd, const sockaddr_in& peerAddr)
{
  loop_ -> assertInLoopThread();
  char buf[32];
  snprintf(buf, sizeof buf, "-%s#%d",ipPort_.c_str(), nextConnId_);
  ++nextConnId_;
  std::string connName = name_ + buf;//新连接的名字 server名-监听IP:PORT#连接序号

  LOG_INFO << "Server::newConn[" << name_ << "] - new connction [" << connName <<"] from " << toIpPort(&peerAddr); 

  //创建出是shared_ptr，引用喜加一
  ConnectionPtr conn(new Connection(loop_, connName, getAddr(sockfd), peerAddr, sockfd));//虽然conn有local地址和peer地址，但通信还是靠sockfd完成
  connections_[connName] = conn;

  conn->setConnCallback(connCallback_);
  conn->setMessCallback(messCallback_);
  conn->connEstablished();              //开启对应Channel可读监听并调用connCallback
}
