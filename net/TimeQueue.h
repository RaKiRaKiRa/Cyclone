/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-30 21:30
 * Last modified : 2019-07-01 19:07
 * Filename      : TimeQueue.h
 * Description   : 
 **********************************************************/

#ifndef TIMEQUEUE_H
#define TIMEQUEUE_H
//#include "Timer.h"
#include "Channel.h"
#include "base/AsyncLogging.h"

#include <map>
#include <set>
#include <vector>
#include <sys/timerfd.h>


namespace timer
{
  
  int64_t now();
  std::string timeToString(int64_t tv);
  std::string timeToFormattedString(int64_t tv, bool showMs);

}

class Timer;
class TimerId;
class EventLoop;
//定时器，实际是用一个Channel对timefd的封装,添加与删除定时事件是通过queueInLoop进行，而定时事件触发是timeFd在poll触发后由loop执行handleRead
//Timer作为事件存于一个二叉搜索树std::set中，操作复杂度O(logN),key为pair<int64_t, Time*>,使得Timer按时间排序
class TimeQueue:noncopyable
{
public:
  //Timer作为事件存于一个二叉搜索树std::set中，操作复杂度O(logN),key为pair<int64_t, Time*>  Entry,使得Timer按时间排序
  //Timer这里使用了原始指针，可以考虑改为智能指针unique_ptr?
  typedef std::pair<int64_t, Timer*> Entry;
  typedef std::set<Entry>            TimerList;
  //已超时的事件存于ActiveTimer中，不以时间索引，用于删除定时器
//  typedef std::pair<Timer*, int64_t> ActiveTimer;
//  typedef std::set<ActiveTimer>      ActiveTimerList;
  typedef std::function<void()> TimerCallback;
  
  TimeQueue(EventLoop* loop);
  ~TimeQueue();

  //增/删对应Timer
  TimerId addTimer(TimerCallback cb, int64_t when, double interval = 0); 
  void removeTimer(TimerId id);

private:
  //定时器唤醒后处理事件
  void handleRead();

  //获得所有到期的Timer并将其从timers中删除
  //返回值虽然是return-by-value，但编译器会进行RVO优化
  std::vector<Entry> getExpired(int64_t now);
  void reset(const std::vector<Entry> &expired, int64_t now);
  bool insert(Timer*);

  //通过bind将下列两函数，即实际增删过程，加入loop的pendingFunctors_中，进行操作
  void addTimerInLoop(Timer* timer);
  void removeTimerInLoop(TimerId id);

  //封装timerfd
  int createTimerfd();
  void resetTime(int64_t when);

  bool calling_;

  EventLoop *loop_;
  //当timefd发生超时时，触发事件可读，返回超时次数（从上次调用timerfd_settime()启动开始或上次read成功读取开始），它是一个8字节的unit64_t类型整数
  //如果定时器没有发生超时事件，则read将阻      塞若timerfd为阻塞模式，否则返回EAGAIN 错误
  const int timerfd_;
  Channel timerfdChannel_;

  TimerList timers_;
  TimerList cancelTimers_;//用于应删除但由于于定时器触发阶段，故没有删除，暂存

// ActiveTimer activeTimers_;

};

#endif
