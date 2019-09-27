/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-09-27 22:19
 * Last modified : 2019-09-27 22:28
 * Filename      : chatServer.h
 * Description   : 
 **********************************************************/
#ifndef CHATSERVER_H
#define CHATSERVER_H

#include "../src/serverWithHeartBeat.h"

class chatServer:noncopyable
{
public:
  chatServer(EventLoop* loop, const sockaddr_in& addr, int idleSec, int threadNum);

  void start()
  {
    server_.start();
  }

private:
  void onConnection(const ConnectionPtr& conn);

  void onMessage(const ConnectionPtr& conn, Buffer* buf);

  serverWithHeartBeat server_;
};


#endif
