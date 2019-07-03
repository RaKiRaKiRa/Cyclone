/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-07 17:39
 * Last modified : 2019-07-03 20:34
 * Filename      : EventLoop.cc
 * Description   : 
 **********************************************************/

#include "EventLoop.h"
#include "IPoller.h"
#include "Channel.h"
#include "TimeQueue.h"

#include <sys/eventfd.h>
// 当前线程中的EventLoop, 保证one loop per thread
__thread EventLoop *t_loopInThisThread = 0;

int createEventfd()
{
  int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if(fd < 0)
  {
    LOG_ERROR << "Failed in eventfd";
    abort();
  }
  return fd;
}

EventLoop::EventLoop(int timeout, poller type):
  PollerType(type),
  kPollTimeOut(timeout),
  loop_(false),
  quit_(true),
  handling_(false),
  threadId_(CurrentThread::tid()),
  poller_(IPoller::newPoller(this)),
  timeQueue_(new TimeQueue(this)),
  wakeupFd_(createEventfd()),
  wakeupChannel_(new Channel(this, wakeupFd_)),
  calling_(false)
{
  LOG <<"EventLoop created " << this <<" in thread " << threadId_;  

  if(t_loopInThisThread)
  {
    LOG << "Another EventLoop " << t_loopInThisThread << "exists in this thread " << threadId_;
  }
  else
  {
    t_loopInThisThread = this;
  }
  wakeupChannel_ -> setReadCallback(std::bind(&EventLoop::handleRead, this));
  wakeupChannel_ -> enableRead();
}

EventLoop::~EventLoop()
{
  assert(!loop_);
  t_loopInThisThread = NULL;
}

EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
  return t_loopInThisThread;
}

void EventLoop::assertInLoopThread()
{
  if(!isInLoopThread())
  {
    abortNotInLoopThread();
  }
}

void EventLoop::abortNotInLoopThread()
{
  //
}

void EventLoop::loop()
{
  //loop()只应该启动一次并在EventLoop所在线程进行
  assert(!loop_);
  assertInLoopThread();
  loop_ = true;
  quit_ = false;
  //以quit_为循环条件保证可以及时退出
  while(!quit_)
  {
    activeChannels_.clear();
    poller_ -> poll(kPollTimeOut, &activeChannels_); 
    handling_ = true;
    for(Channel* it : activeChannels_)
    {
      it -> handleEvent();//it指向Channel*,   *it获得Channel*
    }
    handling_ = false;

  }

  LOG_TRACE <<"EventLoop " << this <<" stop looping";
  loop_ = false;
}

void EventLoop::quit()
{
  quit_ = true;
}

//对Channel操作
void EventLoop::updateChannel(Channel* channel)
{
  assert(channel -> loop() == this);
  assertInLoopThread();
  poller_ -> updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
  assert(channel -> loop() == this);
  assertInLoopThread();
  poller_ -> removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel) const
{
  return poller_ -> hasChannel(channel);
}


//对定时器操作
TimerId EventLoop::runAt(const int64_t& time, TimerCallback cb)
{

}

TimerId EventLoop::runAfter(double delay, TimerCallback cb)
{

}

TimerId EventLoop::runEvery(double interval, TimerCallback cb)
{

}

void EventLoop::runInLoop(Functor cb)
{

}

void EventLoop::queueInLoop(Functor cb)
{

}

void EventLoop::cancel(TimerId timerId)
{

}

void EventLoop::wakeup()
{

}

void EventLoop::handleRead()
{

}

void EventLoop::doPendingFunctors()
{

}
