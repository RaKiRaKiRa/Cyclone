/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-05-29 17:12
 * Last modified : 2019-05-29 20:21
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

class Thread : noncopyable
{
public:
  typedef std::function<void ()> ThreadFunc;
  explicit Thread(ThreadFunc, const std::string& name = std::string());
  ~Thread();

  void start();
  int join();

  bool started() const {return started_; }
  bool joined() const { return joined_; }
  pid_t tid() const { return tid_;  }

  //static int numCreated() { return numCreated_;  }
private:
  bool started_;
  bool joined_;

  pthread_t pthreadId_;
  pid_t tid_;
  ThreadFunc func_;
  std::string name_;

  void setDefaultName();
  CountDownLatch latch_;
  static std::atomic<int> numCreated_ ;
};
#endif
