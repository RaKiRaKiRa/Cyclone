#ifndef SINGLETON_H
#define SINGLETON_H

#include "MutexLock.h"

template<typename T>
class Singleton
{
public:
  static T& Instance()
  {
    if(obj_ == NULL)
    {
      MutexGaurd lock(mutex_);
      if(obj_ == NULL)
      {
        obj_ = new T();
        atexit(&Singleton::Destroy);
      }
    }// unlock
    return *obj_;
  }

private:
  static void Destroy()
  {
    if(obj_ != NULL)
      delete obj_;
    obj_ = NULL;
  }

  Singleton(){}
  ~Singleton(){}
  Singleton(const Singleton& obj) = delete;
  Singleton& operator=(const Singleton& obj) = delete;

  static T* volatile obj_;
  static MutexLock mutex_;
};

template<typename T>
T* Singleton<T>::obj_ volatile = NULL;

template<typename T>
MutexLock Singleton<T>::mutex_;
#endif
