/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-29 14:26
 * Last modified : 2019-07-29 22:27
 * Filename      : EventLoopThreadPool.cc
 * Description   : 
 **********************************************************/

#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "EventLoopThread.h"
#include <stdio.h>

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseloop, const std::string& name):
  baseloop_(baseloop),
  serverName_(name),
  started_(false),
  threadNum_(0),
  next_(0)
{}

EventLoopThreadPool::~EventLoopThreadPool()
{}

void EventLoopThreadPool::start()
{
  assert(!started_);
  baseloop_ -> assertInLoopThread();

  started_ = true;
  // 创建工作线程
  for(int i = 0; i < threadNum_; ++i)
  {
    char buf[serverName_.size() + 10];
    snprintf(buf, sizeof buf, "%s%d", serverName_.c_str(), i);
    EventLoopThread* thread = new EventLoopThread(buf);
    // FIX : move？
    threads_.push_back( std::move( std::unique_ptr<EventLoopThread>(thread) ));
    // 主线程调用start()后，子线程detail::startThread => ThreadData::runInThread => func_(),即EventLoopThread::threadFunc()，停留在loop.loop()上
    workloops_.push_back(thread -> start());
  }
}

EventLoop* EventLoopThreadPool::nextLoop()
{
  baseloop_ -> assertInLoopThread();
  assert(started_);
  // 当threadNum为0时，返回baseloop_即单线程模式
  EventLoop *loop = baseloop_;
  // round-robin``
  if(!workloops_.empty())
  {
    loop = workloops_[next_];
    ++next_;
    if(static_cast<size_t>(next_) >= workloops_.size())
      next_ = 0;
  }
  return loop;
}
