/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-09-05 18:10
 * Last modified : 2020-02-08 15:58
 * Filename      : webServer.cc
 * Description   : 
 **********************************************************/

#include "../Httpserver.h"
#include "../EventLoop.h"
#include "../base/AsyncLogging.h"
#include "../base/Daemon.h"

AsyncLogging *logptr = NULL;
void output(const char* logline, int len)
{
  logptr -> append(logline, len);
}

int main()
{
  Daemon();
  logptr = new AsyncLogging("log", 50*1024*1000);
  Logger::setOutput(output);
  logptr -> start();
  setLogLevel(Logger::INFO);
  EventLoop loop;
  sockaddr_in lis = fromPort(80);
  httpServer myServer(&loop, lis, 2, 60);
  myServer.start();
  loop.loop();
}
