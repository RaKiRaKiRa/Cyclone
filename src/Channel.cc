/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-18 14:24
 * Last modified : 2019-06-18 15:13
 * Filename      : Channel.cc
 * Description   : 
 **********************************************************/

#include "Channel.h"
#include <poll.h>
#include <sys/epoll.h>
#include "base/AsyncLogging.h"

const int Channel::k_None  = 0;
const int Channel::k_Read  = POLLIN | POLLPRI;
const int Channel::k_Write = POLLOUT;

Channel::Channel(EventLoop* loop, int fd):
  loop_(loop),
  fd_(fd),
  events_(0),
  revents_(0),
  handling(false)
{
}

Channel::~Channel()
{
  assert(!handling);
}

//分发事件
void Channel::handleEvent()
{
  handling = true;
  //描述符不合法事件
  if(revents_ & POLLNVAL)
  {
    LOG_WARN <<"Channel::handleEvent() POLLNVAL, fd = " << fd_;
  }

  //错误事件
  if(revents_ & (POLLERR | POLLNVAL))
  {
    if(errorCallback_)
      errorCallback_();
  }

  //读事件,POLLRDHUP即远端挂断连接
  if(revents_ & (POLLIN | POLLPRI | POLLRDHUP))
  {
    if(readCallback_)
      readCallback_();
  }

  //写事件
  if(revents_ & POLLOUT)
  {
    if(writeCallback_)
      writeCallback_();
  }
  handling = false;
}
