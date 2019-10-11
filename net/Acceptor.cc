/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-12 17:21
 * Last modified : 2019-07-18 01:00
 * Filename      : Acceptor.cc
 * Description   : 
 **********************************************************/

#include "Acceptor.h"

Acceptor::Acceptor(EventLoop *loop, const sockaddr_in& addr, bool reuseport):
  loop_(loop),
  socket_(createNonblockingSockfd()),
  channel_(loop, socket_.fd()),
  listening_(false)
{
  //多路复用+回调绑定
  socket_.setReuseAddr(true);
  //开启端口复用后，对于端口bind，如果这个地址/端口处于TIME_WAIT，也可bind成功
  socket_.setReusePort(reuseport);
  socket_.bind(&addr);
  //设置读事件触发后的回调函数，用于调用newConnCallback_,建立一个新Connection
  //此时还没有开始监听Channel，需要在listen中开启enableRead
  channel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

// 将channel从loop和poll移除
Acceptor::~Acceptor()
{
  channel_.disableAll();
  channel_.remove();
}

//开始监听
void Acceptor::listen()
{
  loop_ -> assertInLoopThread();
  listening_ = true;
  socket_.listen();
  //开始监听（加入poll）
  //Acceptor::listen() => Channel::enableRead() => Channel::update() => EventLoop::channelUpdate() => Poller::ChannelUpdate()
  //即channel要加入poller的话必须通过EventLoop
  channel_.enableRead();
}

// 可读（accept）的回调,已有连接完成三次握手
void Acceptor::handleRead()
{
  loop_ -> assertInLoopThread();
  sockaddr_in addr;
  //获得链接对应fd
  //这里加上循环，accept到不能再accept，可以增加对短连接的接受率
  while(1)
  {
    //获得一个链接fd
    int connfd = socket_.accept(&addr);
    if(connfd > 0)
    {
      if(newConnCallback_)
      {
        newConnCallback_(connfd, addr);
      }
      else
      {
        Close(connfd);
      }
    }
    //no more
    else if(errno == EAGAIN || errno == EWOULDBLOCK)
    {
      LOG_TRACE << "has accepted all conn in Acceptor::handleRead";
      break;
    }
    //表示连接失败，出现错误 
    else
    {
      LOG_ERROR << "in Acceptor::handleRead";
      break;
    }
  } 

}

