/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-09-27 22:28
 * Last modified : 2019-09-27 22:55
 * Filename      : chatServer.cc
 * Description   : 
 **********************************************************/

#include "chatServer.h"
#include "Service.h"
#include "../src/base/Singleton.h"

chatServer::chatServer(EventLoop *loop, const sockaddr_in& addr, int idleSec, int threadNum):
  server_(loop, addr, "chatServer", true, idleSec)
{
  Singleton<Service>::Instance();
  server_.setMessCallback(std::bind(&chatServer::onMessage, this, _1, _2));
  server_.setConnCallback(std::bind(&chatServer::onConnection, this, _1));
  server_.setThreadNum(threadNum);
}

void chatServer::onConnection(const ConnectionPtr& conn)
{
  
}

void chatServer::onMessage(const ConnectionPtr& conn, Buffer* buf)
{

}
