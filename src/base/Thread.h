/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-05-29 17:12
 * Last modified : 2019-08-09 22:44
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


// 对线程内实际运行函数的封装， 在执行真正函数前对线程信息进行初始化
struct ThreadData
{
  typedef Thread::ThreadFunc ThreadFunc;
  ThreadFunc func_;
  std::string name_;
  pid_t* tid_;
  CountDownLatch* latch_;

  ThreadData(ThreadFunc func, const std::string &name, pid_t* tid, CountDownLatch* latch):
    func_(std::move(func)),
    name_(name),
    tid_(tid),
    latch_(latch)
  {}
  
  void runInThread();
}; //ThreadData
#endif
