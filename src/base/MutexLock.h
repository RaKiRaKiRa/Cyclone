/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-05-29 17:16
 * Last modified : 2019-05-29 17:16
 * Filename      : MutexLock.h
 * Description   : 
 **********************************************************/
#ifndef MUTEXLOCK_H
#define MUTEXLOCK_H

#include <pthread.h>
#include "noncopyable.h"
class MutexLock : noncopyable 
{
public:
  MutexLock()
  {
    pthread_mutex_init(&mutex, NULL);
  }
  ~MutexLock()
  {
    pthread_mutex_lock(&mutex);
    pthread_mutex_destroy(&mutex);
  }

  void lock()
  {
    pthread_mutex_lock(&mutex);
  }
  void unlock()
  {
    pthread_mutex_unlock(&mutex);
  }

  pthread_mutex_t *get(){
    return &mutex;
  }
  
private:
  friend class Condition;
  pthread_mutex_t mutex;
};

class MutexGuard : noncopyable
{
public:
  explicit MutexGuard(MutexLock& mutex):
    mutex_(mutex)
  {
    mutex_.lock();
  }

  ~MutexGuard()
  {
    mutex_.unlock();
  }
private:
  MutexLock &mutex_;
};

#endif
