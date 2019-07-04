/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-07 17:39
 * Last modified : 2019-07-04 22:47
 * Filename      : EventLoop.cc
 * Description   : 
 **********************************************************/

#include "EventLoop.h"
#include "IPoller.h"
#include "Channel.h"
#include "TimeQueue.h"

#include <sys/eventfd.h>
// 当前线程中的EventLoop, 保证one loop per thread
__thread EventLoop *t_loopInThisThread = 0;

int createEventfd()
{
  int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if(fd < 0)
  {
    LOG_ERROR << "Failed in eventfd";
    abort();
  }
  return fd;
}

EventLoop::EventLoop(int timeout, poller type):
  PollerType(type),
  kPollTimeOut(timeout),
  loop_(false),
  quit_(true),
  handling_(false),
  threadId_(CurrentThread::tid()),
  poller_(IPoller::newPoller(this)),
  timeQueue_(new TimeQueue(this)),
  wakeupFd_(createEventfd()),
  wakeupChannel_(new Channel(this, wakeupFd_)),
  calling_(false)
{
  LOG <<"EventLoop created " << this <<" in thread " << threadId_;  

  if(t_loopInThisThread)
  {
    LOG << "Another EventLoop " << t_loopInThisThread << "exists in this thread " << threadId_;
  }
  else
  {
    t_loopInThisThread = this;
  }
  wakeupChannel_ -> setReadCallback(std::bind(&EventLoop::handleRead, this));
  wakeupChannel_ -> enableRead();
}

EventLoop::~EventLoop()
{
  assert(!loop_);
  t_loopInThisThread = NULL;
}

EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
  return t_loopInThisThread;
}

void EventLoop::assertInLoopThread()
{
  if(!isInLoopThread())
  {
    abortNotInLoopThread();
  }
}

void EventLoop::abortNotInLoopThread()
{
  //
}

void EventLoop::loop()
{
  //loop()只应该启动一次并在EventLoop所在线程进行
  assert(!loop_);
  assertInLoopThread();
  loop_ = true;
  quit_ = false;
  //以quit_为循环条件保证可以及时退出
  while(!quit_)
  {
    activeChannels_.clear();
    poller_ -> poll(kPollTimeOut, &activeChannels_); 

    handling_ = true;
    for(Channel* it : activeChannels_)
    {
      it -> handleEvent();//it指向Channel*,   *it获得Channel*
    }
    handling_ = false;

    doPendingFunctors();
  }

  LOG_TRACE <<"EventLoop " << this <<" stop looping";
  loop_ = false;
}

void EventLoop::quit()
{
  quit_ = true;
}

//****************************************************对Channel操作*****************************************************************************************//

void EventLoop::updateChannel(Channel* channel)
{
  assert(channel -> loop() == this);
  assertInLoopThread();
  poller_ -> updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
  assert(channel -> loop() == this);
  assertInLoopThread();
  poller_ -> removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel) const
{
  return poller_ -> hasChannel(channel);
}


//****************************************************对定时器操作*******************************************************************************************//


TimerId EventLoop::runAt(const int64_t& time, TimerCallback cb)
{
  return timeQueue_ -> addTimer(std::move(cb), time, 0.0);
}

TimerId EventLoop::runAfter(double delay, TimerCallback cb)
{
  int64_t when = timer::now() + static_cast<int64_t>(delay * kMsPerS);
  return runAt(when, std::move(cb));
}

TimerId EventLoop::runEvery(double interval, TimerCallback cb)
{
  int64_t when = timer::now() + static_cast<int64_t>(interval * kMsPerS);
  return timeQueue_ -> addTimer(std::move(cb), when, interval);
}

void EventLoop::runInLoop(Functor cb)
{
  // 若在loop线程中 直接执行
  if(isInLoopThread())
  {
    cb();
  }
  // 否则加入pendingFunctors_ 由loop线程执行
  else
  {
    queueInLoop(std::move(cb));
  }
}

//将cb加入pendingFunctors_使其在loop中执行，加入后对poll进行唤醒
void EventLoop::queueInLoop(Functor cb)
{
  //加入队列
  {
    MutexGuard lock(mutex_);
    pendingFunctors_.push_back(std::move(cb));
  }
  // 唤醒
  /* EventLoop::Wakeup()函数一旦被执行，则一定是由其他线程传入回调函数以后才唤醒的
   * 这也是Wakeup设计的初衷（让其他线程去唤醒），因为EventLoop一旦阻塞，则当前线程阻塞，
   * 只能由其他线程唤醒。所以只要检测到是其他线程在调用当前EventLoop的函数，则进行唤醒。
   * 当calling_为true的时候，说明EventLoop的IO线程执行EventLoop::Loop()中
   * 的DoPendingFunctors()，那就说明了执行if (!IsInLoopThread() || calling_)这个语句的
   * 一定不是EventLoop自己的IO线程（创建EventLoop对象的线程），因为EventLoop自己的IO线程正在执行
   * DoPendingFunctors()函数。所以进行唤醒...
   */
  if(calling_ || !isInLoopThread())
  {
    wakeup();
  }

}

void EventLoop::cancel(TimerId timerId)
{
  timeQueue_ -> removeTimer(timerId);
}

//像wakeupFd_发送一个字节，使poll从阻塞中唤醒
void EventLoop::wakeup()
{
  uint64_t one = 1;
  ssize_t n = read(wakeupFd_, &one, sizeof one);
  if(n != sizeof one)
  {
    LOG_ERROR << "EventLoop::wakeup() writes "<< n << " bytes instead of 8";
  }

}

// 绑定给wakeupChannel_作为接收到wakeup()发送的一个字节的唤醒回调
void EventLoop::handleRead()
{
  uint64_t one = 1;
  ssize_t n = write(wakeupFd_, &one, sizeof one);
  if(n != sizeof one)
  {
    LOG_ERROR << "EventLoop::handleRead() reads "<<n<<" byte instead of 8";
  }

}

// 执行由queueInQueue加入的函数，比如addTimerInLoop, removeTimerInLoop等。  
void EventLoop::doPendingFunctors()
{
  std::vector<Functor> functors;
  calling_ = true;
  {
    MutexGuard lock(mutex_);
    functors.swap(pendingFunctors_);
  }

  for(const Functor f : functors)
  {
    f();
  }

  calling_ = false;
}
