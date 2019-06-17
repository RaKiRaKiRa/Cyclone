/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-05-29 17:30
 * Last modified : 2019-05-29 20:21
 * Filename      : Condition.h
 * Description   : 
 **********************************************************/

#ifndef CONDITION_H
#define CONDITION_H
#include "MutexLock.h"
#include <stdint.h>
class Condition : noncopyable 
{
public:
  explicit Condition(MutexLock& mutex):
    mutex_(mutex)
  {
    pthread_cond_init(&cond_, NULL);
  }
  ~Condition()
  {
    pthread_cond_destroy(&cond_);
  }
  void wait()
  {
    pthread_cond_wait(&cond_, mutex_.get());
  }
  void notify()
  {
    pthread_cond_signal(&cond_);
  }
  void notifyAll()
  {
    pthread_cond_broadcast(&cond_);
  }

  bool waitForSeconds(double sec)
  {
    timespec abstime;
    //CLOCK_REALTIME 统当前时间，从1970年1.1日算起
    //CLOCK_MONOTONIC 系统的启动时间，不能被设置
    //CLOCK_PROCESS_CPUTIME_ID 本进程运行时间
    //CLOCK_THREAD_CPUTIME_ID 本线程运行时间
  clock_gettime(CLOCK_REALTIME, &abstime);
  const int64_t kNanoSecPerSec = 1000000000;
  int64_t nanosec = static_cast<int64_t>(sec * kNanoSecPerSec);

  abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanosec) / kNanoSecPerSec); 
  abstime.tv_nsec += static_cast<long>((abstime.tv_nsec + nanosec) % kNanoSecPerSec); 

  return pthread_cond_timedwait(&cond_, mutex_.get(), &abstime);
  }
private:
  pthread_cond_t cond_;
  MutexLock& mutex_;
};

#endif
