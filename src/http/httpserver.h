/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-27 19:21
 * Last modified : 2019-08-27 20:29
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

  // receive and analyze request
  void getRequest(const ConnectionPtr& conn);

  // default function of getting response by resquest
  static void fillResponse(httpRequest& request, httpResponse* response);

  // get response by resquest
  httpCallback httpCallback_;

  serverWithHeartBeat server_;

};




#endif
