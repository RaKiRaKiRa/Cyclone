/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-29 14:02
 * Last modified : 2019-07-29 15:15
 * Filename      : EventLoopThreadPool.h
 * Description   : 
 **********************************************************/

#ifndef EVENTLOOPTHREADPOOL_H
#define EVENTLOOPTHREADPOOL_H

#include "base/noncopyable.h"
#include <functional>
#include <memory>
#include <vector>

class EventLoopThread;
class EventLoop;
/* 线程池是由Server启动的，在Server::start函数中（由用户调用）EventLoopThreadPool
 * 线程池所在线程在每创建一个EventLoopThread后会调用相应对象的start函数，注意主线程和子线程之分
 * 主线程是Server所在线程，也是线程池所在线程
 * 子线程是由线程池通过pthread_create创建的线程，每一个子线程运行一个EventLoop::loop 
 * 
 * 1.主线程EventLoopThreadPool创建EventLoopThread对象
 * 2.主线程EventLoopThread构造函数中初始化线程类Thread并传递回调函数EventLoopThread::threadFunc
 * 3.主线程EventLoopThreadPool创建完EventLoopThread后，调用EventLoopThread::start函数
 * 4.主线程EventLoopThread::start函数开启线程类Thread，即调用Thread::start
 * 5.主线程Thread::start函数中使用pthread_create创建线程后
 *   子线程调用回调函数EventLoopThread::threadFunc，主线程返回到EventLoopThread::start
 * 6.主线程EventLoopThread::start由于当前事件驱动循环loop_为null（构造时初始化为null）导致wait
 * 7.子线程EventLoopThread::threadFunc创建EventLoop并赋值给loop_，然后唤醒阻塞在cond上的主线程
 * 8.主线程EventLoopThread::start被唤醒后，返回loop_给EventLoopThreadPool
 * 9.主线程EventLoopThreadPool保存返回的loop_，存放在成员变量std::vector<EventLoop*> workloops_中
 * 10.子线程仍然在threadFunc中，调用EventLoop::loop函数，无限循环监听
 */
class EventLoopThreadPool : noncopyable
{
public:
  EventLoopThreadPool(EventLoop *baseloop, const std::string& serverName);
  
  ~EventLoopThreadPool();
  
  void setThreadNum(int num)
  {
    threadNum_ = num;
  } 

  void start();

  // 通过round-robin提取工作线程
  EventLoop* nextLoop();

private:

  EventLoop* baseloop_;
  std::string serverName_;
  bool started_;
  int threadNum_;
  int next_;
  std::vector<EventLoop*> workloops_;
  std::vector<std::unique_ptr<EventLoopThread> > threads_;
};



#endif
