/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-05-31 16:47
 * Last modified : 2019-05-31 17:03
 * Filename      : BlockingQueue.h
 * Description   : 
 **********************************************************/

#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#include "Condition.h"
#include <deque>
#include <assert.h>

template<typename T>
class BlockingQueue : noncopyable
{
public:
  BlockingQueue():
    mutex_(),
    notEmpty_(mutex_),
    queue_()
  {}

  void put(const T& x)
  {
    MutexGuard lock(mutex_);
    queue_.push_back(std::move(x));
    notEmpty_.notify();
  }
 
  T take()
  {
    MutexGuard lock(mutex_);
    while(queue_.empty())
      notEmpty_.wait();
    assert(!queue_.empty());
    T front(std::move(queue_.front()));
    queue_.pop_front();
    return std::move(front);
  }

  size_t size()
  {
    MutexGuard lock(mutex_);
    return queue_.size();
  }

private:
    mutable MutexLock mutex_;
    Condition notEmpty_;
    std::deque<T> queue_;
};
#endif
