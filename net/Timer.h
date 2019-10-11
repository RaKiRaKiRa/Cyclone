/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-28 20:39
 * Last modified : 2019-10-07 20:33
 * Filename      : Timer.h
 * Description   : 
 **********************************************************/

#ifndef TIMER_H
#define TIMER_H
#include "base/Atomic.h"
#include <string>
#include <sys/time.h>
#include "Callback.h"

extern const int64_t kMsPerS;

class Timer:noncopyable
{
public:
  typedef std::function<void()> TimerCallback;

  typedef enum
  {
    TIMER_ONCE = 0, //只触发一次
    TIMER_PREIOD    //周期触发多次
  } TimerType; 

  Timer(TimerCallback cb, int64_t now, double interval = 0.0):
    callback_(std::move(cb)),
    expiration_(now),
    repeat_(interval > 0.0 ? TIMER_PREIOD : TIMER_ONCE),
    interval_(interval),
    sequence_(timerCreated_.incrementAndGet())
  {

  }


  //获得时间事件信息
  int64_t expiration() const
  {
    return expiration_;
  }
  int64_t sequence() const 
  {
    return sequence_;
  }
  TimerType repeat() const
  {
    return repeat_;
  }

  //根据TimerType调整expiration_至下一次触发时间或置零
  void restart(int64_t now);

  //运行事件
  void run() const
  {
    callback_();
  }

  //获得已创建Timer数量，用于赋予sequence
  static int64_t timerCreated()
  {
    return timerCreated_.get();
  }

private:
  //类声明中定义的自动为inline
//  int64_t nextEpiration(int64_t now) const;
  int64_t nextEpiration(int64_t now) const
  {
    return now + static_cast<int64_t>(interval_ * kMsPerS);
  }

  TimerCallback callback_;//回调函数
  int64_t expiration_;    //触发时间
  const TimerType repeat_;     //是否周期触发
  const double interval_; // 触发周期
  const int64_t sequence_;// Timer编号，各不相同, 在触发事件相同的情况下区分各Timer
  static atomic<int64_t> timerCreated_;
};

class TimerId
{
public:
  TimerId():timer_(NULL), sequence_(0){}

  TimerId(Timer* timer, int64_t sequence):
    timer_(timer),
    sequence_(sequence)
  {}

  bool isNull()
  {
    return timer_ == NULL;
  }
  void setNull()
  {
    timer_ = NULL;
  }
  friend class TimeQueue;

private:
    Timer* timer_;
    int64_t sequence_;
};

#endif

