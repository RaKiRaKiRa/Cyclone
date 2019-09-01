/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-27 19:40
 * Last modified : 2019-09-01 20:52
 * Filename      : httpserver.cc
 * Description   : 
 **********************************************************/
#include "httpserver.h"
#include "httpContext.h"
#include "httpResponse.h"
#include "../Connection.h"
#include <string.h>

//httpServer(EventLoop* loop, const sockaddr_in &listenAddr,int threadNum = 4,int keepAliveTime = 60, std::string name = "httpServer", bool ReusePort = true);  
//serverWithHeartBeat(EventLoop* loop, const sockaddr_in& listenAddr, const std::string& name = "", bool ReusePort = true, int idleSec = 20);
httpServer::httpServer(EventLoop* loop, const sockaddr_in &lis, int threadNum, int keepAliveTime, std::string name, bool ReusePort):
  server_(loop, lis, name, ReusePort, keepAliveTime)
{
  server_.setThreadNum(threadNum);
  server_.setConnCallback(std::bind(&httpServer::onConnection, this, _1));
  server_.setMessCallback(std::bind(&httpServer::onMessage, this, _1, _2));
  // 默认只处理静态请求
  setHttpCallback(std::bind(&httpServer::staticSourceRequest, _1, _2));
  keepAliveStr = std::to_string(keepAliveTime);
}

void httpServer::start()
{
  LOG_WARN << "HttpServer["<<server_.name()<<"] starts listenning on " << server_.ipPort();
  server_.start();
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
  // 分析完成后，构造并发送响应报文
  if(context -> getAll())
  {
    onRequest(conn, context -> Request());
    context -> reset();
  }
}

// 构造并发送response
void httpServer::onRequest(const ConnectionPtr& conn, httpRequest& request)
{
  const std::string connection = request.header("Connection");
  // http1.1 默认长连接，http1.0默认短连接
  // connection: close时必短连接，http1.0且未表明keep alive时为短连接
  bool close = (connection == "close") || (request.version() == httpRequest::kHttp10 && connection != "Keep-Alive" && connection != "keep-alive");
  httpResponse response(close);

  //告诉浏览器是否提供长连接及连接时长
  response.addHeader("Connection", close ? "Close" : "Keep-Alive");
  if(!close)
  {
    response.addHeader("Keep-Alive", keepAliveStr);
  }

  // 根据request构造response
  httpCallback_(request, &response);

  Buffer buf;
  // 构造response报文并发送
  response.toBuffer(&buf);
  conn -> send(&buf);
  // 短连接则发送后关闭
  if(response.closeConnection())
  {
    conn -> shutdown();
  }

}

// TODO 构造静态请求response
void httpServer::staticSourceRequest(httpRequest& request, httpResponse* response)
{

}
