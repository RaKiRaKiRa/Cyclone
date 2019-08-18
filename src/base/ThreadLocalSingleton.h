/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-13 10:27
 * Last modified : 2019-08-18 20:23
 * Filename      : ThreadLocalSingleton.h
 * Description   : 
 **********************************************************/
#ifndef THREADLOCALSINGLETON_H
#define THREADLOCALSINGLETON_H

#include "MutexLock.h"

template<typename T>
class ThreadLocalSingleton
{
public:
  static T& Instance()
  {
    if(obj_ == NULL)
    {
      MutexGuard lock(mutex_);
      if(obj_ == NULL)
      {
        obj_ = new T();
      }
    }
    return *obj_;
  }

private:
  ThreadLocalSingleton(){}
  ~ThreadLocalSingleton(){}
  ThreadLocalSingleton(const ThreadLocalSingleton& obj) = delete;
  ThreadLocalSingleton& operator=(const ThreadLocalSingleton& obj) = delete;

  static void Destroy()
  {
    if(obj_ != NULL)
      delete obj_;
    obj_ = NULL;
  }

  class Deleter
  {
  public:
    Deleter()
    {
      pthread_key_create(&key_, &ThreadLocalSingleton::Destroy);
    }

    ~Deleter()
    {
      pthread_key_delete(key_);
    }

    void set(T *obj)
    {
      pthread_setspecific(key_, obj);
    }

    pthread_key_t key_;
  };

  static Deleter              del_;
  static __thread MutexLock   mutex_;
  static __thread T*          obj_;
};

template<typename T>
__thread T* ThreadLocalSingleton<T>::obj_ = NULL;

template<typename T>
typename ThreadLocalSingleton<T>::Deleter ThreadLocalSingleton<T>::del_;

template<typename T>
__thread MutexLock ThreadLocalSingleton<T>::mutex_;

#endif
