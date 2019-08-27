/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-27 19:40
 * Last modified : 2019-08-27 19:40
 * Filename      : httpserver.cc
 * Description   : 
 **********************************************************/
#include "httpserver.h"
#include "httpContext.h"
#include "../Connection.h"

httpServer::httpServer(EventLoop* loop, const sockaddr_in &lis):
  server_(loop, lis, "httpServer")
{
  server_.setConnCallback(std::bind(&httpServer::onConnection, this, _1));
  server_.setMessCallback(std::bind(&httpServer::onMessage, this, _1, _2));
  setHttpCallback(std::bind(&httpServer::fillResponse, _1, _2));
}


void httpServer::onConnection(const ConnectionPtr& conn)
{
  if(conn -> connected())
  {
    conn -> setAnyPtr(new httpContext());
  }
  else
  {
    httpContext *context = static_cast<httpContext*>(conn -> getAnyPtr());
    delete context;
  }
}

void httpServer::onMessage(const ConnectionPtr& conn, Buffer* buf)
{


}


