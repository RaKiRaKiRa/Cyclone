/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-09 21:49
 * Last modified : 2019-07-29 14:00
 * Filename      : EventLoopThread.cc
 * Description   : 
 **********************************************************/

#include "EventLoopThread.h"

EventLoopThread::EventLoopThread(std::string name):
  loop_(NULL),
  exiting_(false),
  thread_(std::bind(&EventLoopThread::threadFunc, this), name),
  mutex_(),
  cond_(mutex_)
{
}

EventLoopThread::~EventLoopThread()
{
  exiting_ = true;
  if(loop_ != NULL)
  {
    loop_ -> quit();
    thread_.join();
  }
}

void EventLoopThread::threadFunc()
{
  EventLoop loop;
  //通知EventLoop创建完毕
  {
    MutexGuard lock(mutex_);
    loop_ = &loop;
    cond_.notify();
  }

  //线程启动，循环于loop()
  loop_ -> loop();

  // 该函数结束后，即loop()退出，loop_失效，因为EventLoop与该函数生命周期相同
  MutexGuard lock(mutex_);
  loop_ = NULL;
}

EventLoop* EventLoopThread::start()
{
  assert(!thread_.started());
  //start() 中会调用threadFunc(), 其会建立EventLoop并赋值给loop_，使start()函数成功退出
  thread_.start();
  //等待EventLoop建立完毕
  {
    MutexGuard lock(mutex_);
    while(loop_ == NULL)
    {
      cond_.wait();
    }
  }

  return loop_;
}
