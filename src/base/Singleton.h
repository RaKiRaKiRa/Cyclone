/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-15 15:46
 * Last modified : 2019-09-06 18:53
 * Filename      : Singleton.h
 * Description   : 
 **********************************************************/
#ifndef SINGLETON_H
#define SINGLETON_H

#include "MutexLock.h"

template<typename T>
class Singleton
{
public:
  static T& Instance()
  {
    T* tmp = obj_;
    if(tmp == NULL)
    {
      MutexGuard lock(mutex_);
      tmp = obj_;
      if(tmp == NULL)
      {
        tmp = new T();
        atexit(&Singleton::Destroy);
        obj_ = tmp;
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

  static T*  obj_;
  static MutexLock mutex_;
};

template<typename T>
T* Singleton<T>::obj_ = NULL;

template<typename T>
MutexLock Singleton<T>::mutex_;
#endif
