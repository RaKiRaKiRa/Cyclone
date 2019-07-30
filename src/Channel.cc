/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-18 14:24
 * Last modified : 2019-07-17 00:37
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
  //描述符未开事件
  if(revents_ & POLLNVAL)
  {
    LOG_WARN <<"Channel::handleEvent() POLLNVAL, fd = " << fd_;
  }

  //对方挂起？且没有可读数据，则关闭
  if((revents_ & POLLHUP) && !(revents_ & POLLIN))
  {
    LOG_WARN << "fd = " << fd_ << " Channel::handle_event() POLLHUP";
    if(closeCallback_)
      closeCallback_();
  }

  //错误事件或文件描述符未开
  if(revents_ & (POLLERR | POLLNVAL))
  {
    //LOG_WARN << "pollerr | pollnval";
    if(errorCallback_)
      errorCallback_();
  }

  //读事件,
  //若触发且没有读到数据则表明对方关闭写并将数据全部读完，进行closeCallback_，完成优雅关闭
  //POLLRDHUP即远端关闭连接或关闭写操作,当对端正常关闭的时候会触发POLLIN | POLLRDHUP
  if(revents_ & (POLLIN | POLLPRI | POLLRDHUP))
  {
    //LOG_INFO <<" (POLLIN | POLLPRI | POLLRDHUP))";
    if(readCallback_)
      readCallback_();
  }

  //写事件
  if(revents_ & POLLOUT)
  {
    //LOG_INFO <<" pollout ";
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
