/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-07 17:28
 * Last modified : 2019-07-03 20:34
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
#include "Timer.h"

class Channel;
class IPoller;
class TimeQueue;

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
  typedef std::function<void()> TimerCallback;
  typedef std::vector<Channel*> ChannelList;//用于连接Channel

  //选择默认Poller
  EventLoop(int timeout = 10000, poller type = k_epoll);
  ~EventLoop();

  void loop();
  void quit();

  //Channel IPoller TimeQueue不直接相互调用,而是通过EventLoop中的控制函数进行间接交互
  //
  //对Channel操作
  void updateChannel(Channel* channel);
  void removeChannel(Channel* channel);
  bool hasChannel(Channel* channel) const;


  //定时器相关
  TimerId runAt(const int64_t& time, TimerCallback cb);
  TimerId runAfter(double delay, TimerCallback cb);
  TimerId runEvery(double interval, TimerCallback cb);
  void runInLoop(Functor cb);                           //若在loop线程中直接执行cb，否则调用queueInLoop
  void queueInLoop(Functor cb);                         //将cb加入pendingFunctors_使其在loop中执行，加入后对poll进行唤醒
  void cancel(TimerId timerId);
  void wakeup();                                        //像wakeupFd_发送一个字节，使poll从阻塞中唤醒
  void handleRead();                                    //绑定给wakeupChannel_做可读监听，从wakeupFd_接收一个字节，使poll从阻塞状态唤醒
  void doPendingFunctors(); 


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

  std::unique_ptr<TimeQueue> timeQueue_;
  std::vector<Functor> pendingFunctors_;
  int wakeupFd_;
  std::unique_ptr<Channel> wakeupChannel_;
  bool calling_;

  MutexLock mutex_;
};

#endif 
