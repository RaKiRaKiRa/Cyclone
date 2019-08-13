#include "MutexLock.h"

template<typename T>
class ThreadLocalThread
{
public:
  T& Instance()
  {
    if(obj_ == NULL)
    {
      MutexGaurd lock(mutex_)
      if(obj_ == NULL)
      {
        obj_ = new T();
      }
    }
    return *obj_;
  }

private:
  ThreadLocalThread(){}
  ~ThreadLocalThread(){}
  ThreadLocalThread(const ThreadLocalThread& obj) = delete;
  ThreadLocalThread& operator=(const ThreadLocalThread& obj) = delete;

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

  static Deleter     del_;
  static MutexLock   mutex_;
  static __thread T* obj_;
};
