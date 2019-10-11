/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-01 15:35
 * Last modified : 2019-07-04 23:44
 * Filename      : TimeQueue.cc
 * Description   : 
 **********************************************************/

#include "TimeQueue.h"
#include "EventLoop.h"
#include "Timer.h"
#include <algorithm>

#include <string>

namespace timer
{
  int64_t now()
  {
    struct timeval t;
    gettimeofday(&t , NULL);
    return t.tv_usec + t.tv_sec * kMsPerS;
  }

  std::string timeToString(int64_t tv)
  {
    char buf[32];
    snprintf(buf, sizeof buf, "%ld .%ld", tv/kMsPerS, tv%kMsPerS);
    return buf;
  }

  std::string timeToFormattedString(int64_t tv, bool showMs)
  {
    char buf[64];
    /*
      struct tm
      {
           int    tm_sec;//代表目前秒数，正常范围为0-59，但允许至61秒
           int    tm_min;//代表目前分数，范围0-59
           int    tm_hour;
           int    tm_mday;//目前月份的日数，范围01-31
           int    tm_mon;//代表目前月份，从一月算起，范围从0-11
           int    tm_year;//从1900年算起至今的年数
           int    tm_wday;//一星期的日数，从星期一算起，范围为0-6
           int    tm_yday;//从今年1月1日算起至今的天数，范围为0-365
           int    tm_isdst;
      };
   */
    struct tm tv_;
    time_t sec = static_cast<time_t>(tv/kMsPerS);
    gmtime_r(&sec, &tv_);

    if(showMs)
    {
      int ms = static_cast<int>(tv % kMsPerS);
      snprintf(buf, sizeof buf, "%4d/%02d/%02d %02d:%02d:%02d.%06d", 
          tv_.tm_year + 1900, tv_.tm_mon + 1, tv_.tm_mday, 
          tv_.tm_hour, tv_.tm_min, tv_.tm_sec, ms);
      return buf;
    }
    else
    {
      snprintf(buf, sizeof buf, "%4d/%02d/%02d %02d:%02d:%02d", 
          tv_.tm_year + 1900, tv_.tm_mon + 1, tv_.tm_mday, 
          tv_.tm_hour, tv_.tm_min, tv_.tm_sec);
      return buf;
    }
  }

}


int createTimerfd()
{
  int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  if(timerfd < 0)
    LOG_ERROR << "Failed in timerfd_create";
  return timerfd;
}


TimeQueue::TimeQueue(EventLoop* loop):
  calling_(false),
  loop_(loop),
  timerfd_(::createTimerfd()),
  timerfdChannel_(loop, timerfd_),
  timers_(),
  cancelTimers_()
{
  timerfdChannel_.setReadCallback(std::bind(&TimeQueue::handleRead, this));
  timerfdChannel_.enableRead();
}

TimeQueue::~TimeQueue()
{
  //现将对应Channel关闭并移除loop
  timerfdChannel_.disableAll();
  timerfdChannel_.remove();
  //然后删除Timer 防止内存泄露
  for(const Entry& it : timers_)
  {
    delete it.second;
  }
}


//重置定时器时间
void TimeQueue::resetTime(int64_t when)
{
  struct itimerspec newTime;
  struct itimerspec oldTime;
  memset(&newTime, 0, sizeof newTime);
  memset(&oldTime, 0, sizeof oldTime);

  //至少等待100ms，防止触发频率过高
  int64_t ms = when - timer::now();
  if(ms < 100) ms = 100;//单位 微秒

  // 1秒 = 1000毫秒 = 1000000微秒 = 1000000*1000纳秒，timeval单位是秒和微秒，timespec为秒和纳秒
  newTime.it_value.tv_sec = static_cast<time_t>(ms / kMsPerS);//秒
  newTime.it_value.tv_nsec = static_cast<time_t>((ms % kMsPerS) * 1000);//纳秒

  int ret = timerfd_settime(timerfd_, 0, &newTime, &oldTime);
  if(ret < 0)
  {
    LOG_ERROR << "timerfd_settime()";
  }
}


void TimeQueue::handleRead()
{
  loop_ -> assertInLoopThread();

  int64_t now = timer::now();

  uint64_t howmany;
  //当定时器超时，read读事件发生即可读，返回超时次数（从上次调用timerfd_settime()启动开始或上次read成功读取开始）
  ssize_t n = read(timerfd_, &howmany, sizeof howmany);
  LOG_TRACE <<"TimerQueue::handleRead() reads " << n << " at " << timer::timeToString(now);
  if(n != sizeof howmany)
  {
    LOG_ERROR <<"TimerQueue::handleRead() reads "<<n<<" bytes instead of 8";
  }

  //将超时事件删除并返回
  std::vector<Entry> expired = getExpired(now);

  //处理超时事件
  cancelTimers_.clear();
  calling_ = true;
  for(std::vector<Entry>::iterator it = expired.begin(); it != expired.end(); ++it)
  {
    it -> second -> run();
  }
  
  calling_ = false;
  //对TIME_PERIOD类型重新加入，对ONCE进行销毁
  reset(expired, now);
}

//将超时事件删除并返回
//返回值虽然是return-by-value，但编译器会进行RVO优化
std::vector<TimeQueue::Entry> TimeQueue::getExpired(int64_t now)
{
  std::vector<TimeQueue::Entry> expired;
  //哨兵值， 取当前时间与地址最大指针
  Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));//
  //获得第一个比哨兵值大的指针，即未超时的第一个，作为界限
  TimerList::iterator it = timers_.lower_bound(sentry);
  
  //将超时Timer放入expired并删除
  std::copy(timers_.begin(), it, back_inserter(expired));
  timers_.erase(timers_.begin(), it);

  return expired;
}

//对TIME_PERIOD类型重新加入，对ONCE进行销毁
//此函数仅被handleRead调用，事先在getExpired中，对应Entry已经不在timer_中
//故销毁时直接销毁对应Timer即可
void TimeQueue::reset(const std::vector<Entry> &expired, int64_t now)
{
  int64_t nextExpire = 0;  
  for(std::vector<Entry>::const_iterator it = expired.begin(); it != expired.end(); ++it)
  {
    if(it->second->repeat() && cancelTimers_.find(*it) == cancelTimers_.end())
    {
      it->second->restart(now);
      insert(it->second);
    }
    //此函数仅被handleRead调用，事先在getExpired中，对应Entry已经不在timer_中
    //故销毁时直接销毁对应Timer即可
    else
    {
      delete it -> second;
    }
  }

  //然后重置下次定时器反应事件
  //若没有事件则不定时
  if(!timers_.empty())
  {
    nextExpire = timers_.begin() -> second -> expiration();
  }
  if(nextExpire > 0)
  {
    resetTime(nextExpire);
  }
}


bool TimeQueue::insert(Timer* timer)
{
  loop_ -> assertInLoopThread();
  bool changed = false;//用于判断是否需要重新定时，当其触发时间比定时时间更早或当前无任务时置为true

  int64_t when = timer -> expiration();

  TimerList::iterator it = timers_.begin();
  //当其触发时间比定时时间更早或当前无任务时置为true
  if(it == timers_.end() || when < it -> first)
  {
    changed = true;
  }

  // set::insert的返回值为pair型，first为新加入的迭代器，second返回是否之前存在
  std::pair<TimerList::iterator, bool> res = timers_.insert(Entry(when, timer));
  assert(res.second); (void)res;
  return changed;
}


TimerId TimeQueue::addTimer(TimerCallback cb, int64_t when, double interval)
{
  Timer* timer = new Timer(std::move(cb), when, interval);  
  loop_ -> runInLoop(std::bind(&TimeQueue::addTimerInLoop, this, timer));
  return TimerId(timer, timer -> sequence());
}

//使得加入过程在loop线程执行
void TimeQueue::addTimerInLoop(Timer* timer)
{
  loop_ -> assertInLoopThread();
  if(insert(timer))
  {
    resetTime(timer -> expiration());
  }
}


void TimeQueue::removeTimer(TimerId timerId)
{
  loop_ -> runInLoop(std::bind(&TimeQueue::removeTimerInLoop, this, timerId));
}

void TimeQueue::removeTimerInLoop(TimerId timerId)
{
  loop_ ->assertInLoopThread();
  TimerList::iterator it = timers_.find(Entry(timerId.timer_ -> expiration(), timerId.timer_));

  if(it != timers_.end())
  {
    timers_.erase(it);
    delete timerId.timer_;
  }
  else if(calling_)//若没有找到对应定时器且正在处理定时事件，则可能是已被处理进activeTimers_，加入cancelingTimers_表示之后不再加入
  {
    cancelTimers_.insert(Entry(timerId.timer_ -> expiration(), timerId.timer_));
  }
}
