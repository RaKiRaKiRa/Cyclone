/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-07 17:39
 * Last modified : 2019-06-15 21:53
 * Filename      : EventLoop.cc
 * Description   : 
 **********************************************************/

#include "EventLoop.h"
// 当前线程中的EventLoop, 保证one loop per thread
__thread EventLoop *t_loopInThisThread = 0;
//EventLoop中的poll等待毫秒数
const int kPollTImeMs = 10000;


EventLoop::EventLoop():
  loop_(false),
  threadId_(CurrentThread::tid())
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

void EventLoop::loop()
{
  assert(!loop_);
  
}
