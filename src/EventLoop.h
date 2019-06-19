/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-07 17:28
 * Last modified : 2019-06-19 15:46
 * Filename      : EventLoop.h
 * Description   : 
 **********************************************************/

#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "base/noncopyable.h"
#include "base/CurrentThread.h"
#include "base/AsyncLogging.h"
#include <pthread.h>
#include <assert.h>
#include <functional>

class Channel;

//事件分发器
class EventLoop : noncopyable
{
public:
  typedef std::function<void()> Functor;//用于定时器
  typedef std::vector<Channel*> ChannelList;//用于连接Channel

  //选择默认Poller
  enum poller
  {
    k_epoll,
    k_poll
  };


  EventLoop();
  ~EventLoop();

  void loop();

  bool isInLoopThread() const
  {
    return threadId_ == CurrentThread::tid();
  }

  void assertInLoopThread();
  
  static EventLoop* getEventLoopOfCurrentThread();
  static poller DefaultPoller;

private:
  void abortNotInLoopThread();
  ChannelList activeChannels_;//由Poller::poll得到的

  bool loop_;
  const pid_t threadId_;

};
EventLoop::poller EventLoop::DefaultPoller = EventLoop::k_epoll;

#endif 
