/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-27 19:21
 * Last modified : 2019-08-31 21:14
 * Filename      : httpserver.h
 * Description   : 
 **********************************************************/
#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <iostream>
#include "../serverWithHeartBeat.h"

class httpRequest;
class httpResponse;

class httpServer:noncopyable
{
public:
  typedef std::function<void(httpRequest&, httpResponse* )> httpCallback ;

  httpServer(EventLoop* loop, const sockaddr_in &listenAddr);  

  void setThreadNum(int num)
  {
    server_.setThreadNum(num);
  }

  void setHttpCallback(httpCallback cb)
  {
    httpCallback_ = std::move(cb);
  }

  void start();

private:
  // create or destroy httpContent 
  void onConnection(const ConnectionPtr& conn);

  // receive and analyze request, and response
  void onMessage(const ConnectionPtr& conn, Buffer* buf);

  // 构造并发送response
  void onRequest(const ConnectionPtr& conn, httpRequest& request);

  // 默认构造静态response
  static void staticSourceRequest(httpRequest& request, httpResponse* response);

  // 构造请求response
  httpCallback httpCallback_;

  serverWithHeartBeat server_;

};




#endif
