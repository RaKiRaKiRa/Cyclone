/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-27 19:40
 * Last modified : 2019-12-06 02:25
 * Filename      : httpserver.cc
 * Description   : 
 **********************************************************/
#include "Httpserver.h"
#include "HttpContext.h"
#include "HttpResponse.h"
#include "HttpStaticFile.h"
#include "Connection.h"
#include <string.h>
#include <string>
#include <unistd.h>

std::string webPath;

httpServer::httpServer(EventLoop* loop, const sockaddr_in &lis, int threadNum, int keepAliveTime, std::string name, bool ReusePort):
  server_(loop, lis, name, ReusePort, keepAliveTime),
  keepAliveTime_(keepAliveTime)
{
  server_.setThreadNum(threadNum);
  server_.setConnCallback(std::bind(&httpServer::onConnection, this, _1));
  server_.setMessCallback(std::bind(&httpServer::onMessage, this, _1, _2));
  // 默认只处理静态请求
  setHttpCallback(std::bind(&httpServer::staticSourceRequest, _1, _2));
}

void httpServer::start()
{
  LOG_WARN << "HttpServer["<<server_.name()<<"] starts listenning on " << server_.ipPort();
  server_.start();
}

void httpServer::onConnection(const ConnectionPtr& conn)
{
  // 建立连接时创建httpContext
  if(conn -> connected())
  {
    conn -> setAnyPtr(new std::shared_ptr<httpContext>(new httpContext()));
  }
  // 断开连接时析构httpContext
  else
  {
    if(conn -> getAnyPtr() != NULL)
    {
      std::shared_ptr<httpContext>* context = static_cast<std::shared_ptr<httpContext>* >(conn -> getAnyPtr());
      LOG_DEBUG << "disconnection : destruct the context";
      delete context;
      LOG_DEBUG << "disconnection : destructed the context";
      conn -> setAnyPtr(NULL);
    }
  }
}

// 收到消息后进行处理
void httpServer::onMessage(const ConnectionPtr& conn, Buffer* buf)
{
  std::shared_ptr<httpContext> context = *static_cast<std::shared_ptr<httpContext>* >(conn -> getAnyPtr());
  //httpContext* context = static_cast<httpContext*>(conn -> getAnyPtr());
  // 分析请求报文，若出错返回400
  // FIXME:有bug
  if(!context -> parseRequest(buf))
  {
    LOG_WARN << "parseRequest 400 Error";
    conn -> send("HTTP/1.1 400 Bad Request \r\n\r\n");
    conn -> shutdown();
  }
  // 分析完成后，构造并发送响应报文
  if(context -> getAll())
  {
    LOG_DEBUG << "context has got all";
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
  httpResponse response(close, keepAliveTime_);

  // 根据request构造response
  httpCallback_(request, &response);
  LOG_DEBUG<<"response has created";

  /*
  // 可以移动至toBuffer，直接加入buffer
  // 告诉浏览器是否提供长连接及连接时长
  // 不使用close是因为httpCallback可能会导致变化 (NotFound)
  response.addHeader("Connection", response.closeConnection() ? "Close" : "Keep-Alive");
  if(!response.closeConnection())
  {
    response.addHeader("Keep-Alive", keepAliveStr);
  }
  */

  Buffer buf;
  // 构造response报文并发送
  response.toBuffer(&buf);
  conn -> send(&buf);
  LOG_DEBUG << "response has sent";
  // 短连接则发送后关闭
  if(response.closeConnection())
  {
    conn -> shutdown();
  }

}

// 构造静态请求response
void httpServer::staticSourceRequest(httpRequest& request, httpResponse* response)
{
  if(webPath.empty())
  {
    char tmpPath[80];
    getcwd(tmpPath, 80);
    webPath = tmpPath;
  }
  std::string filename = request.path();
  if(filename == "/" || filename == "")
    filename = "/index.html";
  
  filename = webPath + filename;
  StaticFile sFile(filename);
  // 若读取文件进入body成功
  if(sFile.open() && response -> setBody(sFile))
  {
    assert(sFile.state() == StaticFile::OK);
    LOG_WARN << filename << " open&read OK ";
    response -> setStatusCode(httpResponse::k200Ok);
    response -> setStatusMessage("OK");
    response -> addHeader("Server", "Cyclone");
    response -> setContentType(std::move(sFile.contentType()));
  }
  // 失败 发送404NotFount报文
  else
  {
    assert(sFile.state() == StaticFile::NotFound || sFile.state() == StaticFile::ReadError);
    if(sFile.state() == StaticFile::NotFound)
    {
      LOG_WARN << filename << " Not Fount ";
    }
    else if(sFile.state() == StaticFile::ReadError)
    {
      LOG_WARN << filename << " Read Error ";
    }
    response -> setNotFound();
  } 
}
