/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-18 14:24
 * Last modified : 2019-06-30 17:29
 * Filename      : Channel.cc
 * Description   : 
 **********************************************************/

#include "Channel.h"
#include <poll.h>
#include <sys/epoll.h>
#include "base/AsyncLogging.h"
#include "EventLoop.h"

const int Channel::k_None  = 0;
const int Channel::k_Read  = POLLIN | POLLPRI;
const int Channel::k_Write = POLLOUT;

Channel::Channel(EventLoop* loop, int fd):
  loop_(loop),
  fd_(fd),
  events_(0),
  revents_(0),
  handling_(false),
  index_(-1),
  addedToLoop_(false)
{
}

Channel::~Channel()
{
  assert(!handling_);
}

//分发事件
void Channel::handleEvent()
{
  handling_ = true;
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
  handling_ = false;
}

void Channel::update()
{
  addedToLoop_ = true;
  loop_ -> updateChannel(this);
}

//将自身从EventLoop和对应IPoller内删除
void Channel::remove()
{
  assert(isNoneEvent());
  addedToLoop_ = false;
  loop_ -> removeChannel(this);
}
