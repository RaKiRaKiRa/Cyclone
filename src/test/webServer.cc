/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-09-05 18:10
 * Last modified : 2019-09-06 13:29
 * Filename      : webServer.cc
 * Description   : 
 **********************************************************/

#include "../httpserver.h"
#include "../EventLoop.h"

int main()
{
  setLogLevel(Logger::DEBUG);
  EventLoop loop;
  sockaddr_in lis = fromPort(9111);
  httpServer myServer(&loop, lis, 1, 10);
  myServer.start();
  loop.loop();
}
