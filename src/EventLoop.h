/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-07 17:28
 * Last modified : 2019-06-07 17:37
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

class EventLoop : noncopyable
{
public:
  typedef std::function<void()> FuncTor;//用于定时器

  EventLoop();
  ~EventLoop();
  void loop();
  bool isInLoopThread() const
  {
    return threadId_ == CurrentThread::tid();
  }

  void assertInLoopThread();
  static EventLoop* getEventLoopOfCurrentThread();

private:
  void abortNotInLoopThread();

  bool loop_;
  const pid_t threadId_;

};

#endif 
