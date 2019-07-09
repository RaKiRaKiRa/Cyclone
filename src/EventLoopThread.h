/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-04 22:50
 * Last modified : 2019-07-09 21:49
 * Filename      : EventLoopThread.h
 * Description   : 
 **********************************************************/

#ifndef EVENTLOOPTHREAD_H
#define EVENTLOOPTHREAD_H

#include "EventLoop.h"
#include "base/Thread.h"
#include <string>


class EventLoopThread : noncopyable
{
public:
  EventLoopThread(const std::string name = std::string());

  ~EventLoopThread();

  EventLoop* start();

private:
  //新建线程内的实际函数内容
  void threadFunc();

  EventLoop* loop_;

  //是否退出
  bool exiting_;

  //线程控制三兄弟。。。。
  Thread thread_;
  MutexLock mutex_;
  Condition cond_;
};

#endif
