/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-05-29 21:28
 * Last modified : 2019-05-31 16:45
 * Filename      : Thread.cc
 * Description   : 
 **********************************************************/

#include "Thread.h"
#include "CurrentThread.h"
#include <sys/prctl.h>
#include <assert.h>
pid_t gettid()
{
  return static_cast<pid_t>(::syscall(SYS_gettid));
}

namespace CurrentThread
{  
  static_assert(std::is_same<int, pid_t>::value, "pid_t should be int");
  __thread int t_cachedTid = 0;
  __thread char t_tidString[32];
  __thread int t_tidStringLength = 6;
  __thread const char* t_threadName = "default";

  //初始化线程号记录
  void cacheTid()
  {
    if(t_cachedTid == 0)
    {
      t_cachedTid = gettid();
      t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), "%5d ", t_cachedTid);
    }
  }

  bool isMainThread()
  {
    return ::getpid() == tid();
  }
}// CurrentThread

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

  void runInThread()
  {
    *tid_ = CurrentThread::tid();
    tid_ = NULL;//指向空,防止delete时伤及无辜
    latch_->countDown();
    latch_ =NULL;
    CurrentThread::t_threadName = name_.empty() ? "CycloneThread": name_.c_str();
    prctl(PR_SET_NAME, CurrentThread::t_threadName);//设置线程名
    
    func_();//真正的线程执行函数
    CurrentThread::t_threadName = "finished";
  }
};

//Thread::start() => startThread() => ThreadData::runInThread() => ThreadFunc
//所以startThread可以看成是一个跳板或中介,若用成员函数需要用static void*
void* startThread(void *obj)
{
  ThreadData* data = static_cast<ThreadData*>(obj);
  data -> runInThread();
  delete data;
  return NULL;
}

std::atomic<int> Thread::numCreated_(0);

Thread::Thread(ThreadFunc func, const std::string& n):
  started_(false),
  joined_(false),
  pthreadId_(0),
  tid_(0),
  func_(std::move(func)),
  name_(n),
  latch_(1)
{
  setDefaultName();
}

void Thread::setDefaultName()
{
  int num = numCreated_++;
  if(name_.empty())
  {
    char buf[32];
    snprintf(buf, sizeof(buf), "Thread%d", num);
    name_ = buf;
  }
}

//一般使Thread对象的生命期长于线程，然后通过Thread::join()等待线程结束并释放线程资源。
//但如果Thread对象的生命期短于线程，则在析构时触发detach线程
//从而让操作系统在该线程结束时来回收它所占的资源，避免内存泄漏。
Thread::~Thread()
{
  if(started_ && !joined_)
  {
    //为了在使用 pthread 时避免线程的资源在线程结束时不能得到正确释放，
    //从而避免产生潜在的内存泄漏问题，在对待线程结束时，要确保该线程处于 detached 状态，从而让操作系统在该线程结束时来回收它所占的资源。
    //否则就需要调用 pthread_join() 函数来对其进行资源回收。
    pthread_detach(pthreadId_);
  }
}


void Thread::start()
{
  assert(!started_);
  started_ = true;
  ThreadData *data = new ThreadData(std::move(func_), name_, &tid_, &latch_);
  if(pthread_create(&pthreadId_, NULL, &startThread, data ))
  {
    started_ = false;
    delete data;
  }
  else
  {
    latch_.wait();
    assert(tid_ > 0);
  }
}

int Thread::join()
{
  assert(started_);
  assert(!joined_);
  joined_ = true;
  return pthread_join(pthreadId_, NULL);
}
