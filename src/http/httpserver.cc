/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-27 19:40
 * Last modified : 2019-08-31 21:14
 * Filename      : httpserver.cc
 * Description   : 
 **********************************************************/
#include "httpserver.h"
#include "httpContext.h"
#include "httpResponse.h"
#include "../Connection.h"

httpServer::httpServer(EventLoop* loop, const sockaddr_in &lis):
  server_(loop, lis, "httpServer")
{
  server_.setConnCallback(std::bind(&httpServer::onConnection, this, _1));
  server_.setMessCallback(std::bind(&httpServer::onMessage, this, _1, _2));
  setHttpCallback(std::bind(&httpServer::staticSourceRequest, _1, _2));
}


void httpServer::onConnection(const ConnectionPtr& conn)
{
  // 建立连接时创建httpContext,断开后析构
  if(conn -> connected())
  {
    conn -> setAnyPtr(new httpContext());
  }
  else
  {
    httpContext *context = static_cast<httpContext*>(conn -> getAnyPtr());
    if(context != NULL)
      delete context;
  }
}

// 收到消息后进行处理
void httpServer::onMessage(const ConnectionPtr& conn, Buffer* buf)
{
  httpContext* context = static_cast<httpContext*>(conn -> getAnyPtr());
  // 分析出错，返回400
  if(!context -> parseRequest(buf))
  {
    conn -> send("HTTP/1.1 400 Bad Request \r\n\r\n");
    conn -> shutdown();
  }
  // 分析完成
  if(context -> getAll())
  {
    // 构造并发送response
    onRequest(conn, context -> Request());
    context -> reset();
  }
}

// 构造并发送response
void httpServer::onRequest(const ConnectionPtr& conn, httpRequest& request)
{
  httpResponse response;
  // 构造response
  httpCallback_(request, &response);

  Buffer buf;
  // 构造response报文
  response.toBuffer(&buf);
  conn -> send(&buf);

}

// 构造response
void httpServer::staticSourceRequest(httpRequest& request, httpResponse* response)
{

}
