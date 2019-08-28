/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-19 15:44
 * Last modified : 2019-08-19 15:44
 * Filename      : echo_heart.cc
 * Description   : 
 **********************************************************/


#include "../EventLoopThreadPool.h"
#include "../base/Logging.h"
#include "../serverWithHeartBeat.h"
#include "../EventLoop.h"

#include <stdio.h>
#include <unistd.h>

int numThreads = 0;


AsyncLogging *logptr = NULL;
void output(const char* logline, int len)
{
  logptr -> append(logline, len);
}

class EchoServer
{
public:
  EchoServer(EventLoop* loop, const sockaddr_in& addr):
    server_(loop, addr, "EchoServer")
  {
    server_.setConnCallback(std::bind(&EchoServer::onConnection, this, _1));
    server_.setMessCallback(std::bind(&EchoServer::onMessage, this, _1, _2));
    server_.setThreadNum(numThreads);
  }

  void start()
  {
    LOG_INFO << "starting " << numThreads <<" threads";
    server_.start();
  }

  void onConnection(const ConnectionPtr& conn)
  {
    LOG_TRACE << toIpPort(conn -> peer()) << " -> " <<toIpPort(conn -> local())<<" is " <<(conn -> connected()?"UP":"DOWN");
    conn -> setNoDelay(true);
  }

  // buf一般是Connection::inputBuffer_
  void onMessage(const ConnectionPtr& conn, Buffer* buf)
  {
    std::string s(buf -> toString());
    conn -> send(s);
  }

private:
    serverWithHeartBeat server_;
};



int main(int argc, char* argv[])
{
  logptr = new AsyncLogging("asynclog", 50*1024*1000);
  Logger::setOutput(output);
  logptr -> start();

  LOG_INFO <<"PID = " << getpid() <<", tid = " << CurrentThread::tid();
  if(argc > 1)
    numThreads = atoi(argv[1]);
  EventLoop loop;
  sockaddr_in addr;
  fromPort(4567, &addr);
  EchoServer server(&loop, addr);
  server.start();

  loop.loop();

}
