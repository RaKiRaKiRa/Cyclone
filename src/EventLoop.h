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
class IPoller;

enum poller
{
  k_epoll,
  k_poll
};

//事件分发器
class EventLoop : noncopyable
{
public:
  typedef std::function<void()> Functor;//用于定时器
  typedef std::vector<Channel*> ChannelList;//用于连接Channel

  //选择默认Poller
  EventLoop(int timeout = 10000, poller type = k_epoll);
  ~EventLoop();

  void loop();
  void quit();

  //Channel IPoller TimeQueue不直接相互调用,而是通过EventLoop中的控制函数进行间接交互
  void updateChannel(Channel* channel);
  void removeChannel(Channel* channel);
  bool hasChannel(Channel* channel) const;

  // 用于断言是否在Loop线程中进行
  bool isInLoopThread() const
  {
    return threadId_ == CurrentThread::tid();
  }

  void assertInLoopThread();
    
  static EventLoop* getEventLoopOfCurrentThread();

  const poller PollerType;
private:
  const int kPollTimeOut;//EventLoop中的poll等待毫秒数
  void abortNotInLoopThread();

  bool loop_;
  bool quit_;
  bool handling_;
  const pid_t threadId_;

  std::unique_ptr<IPoller> poller_;
  ChannelList activeChannels_;//由Poller::poll得到的

};

#endif 
