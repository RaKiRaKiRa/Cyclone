/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-05-29 17:12
 * Last modified : 2019-07-04 22:57
 * Filename      : Thread.h
 * Description   : 
 **********************************************************/
#ifndef THREAD_H
#define THREAD_H

#include "CountDownLatch.h"
#include <string>
#include <sys/syscall.h>
#include <functional>
#include <string>
#include <unistd.h>
#include <atomic>
#include "Atomic.h"

// 对线程信息与线程控制的封装
class Thread : noncopyable
{
public:

  typedef std::function<void ()> ThreadFunc;
  explicit Thread(ThreadFunc, const std::string& name = std::string());
  ~Thread();

  void start();
  int join();

  // 获取线程状态与信息
  bool started() const {return started_; }
  bool joined() const { return joined_; }
  pid_t tid() const { return tid_;  }

  //static int numCreated() { return numCreated_;  }
private:
  void setDefaultName();

  //线程状态标志
  bool started_;
  bool joined_;

  //线程信息
  pthread_t pthreadId_;
  pid_t tid_;
  ThreadFunc func_;//线程实际运行函数
  std::string name_;

  CountDownLatch latch_;
  static std::atomic<int> numCreated_ ;
};
#endif
