/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-05-29 20:37
 * Last modified : 2019-05-29 20:51
 * Filename      : CountDownLatch.h
 * Description   : 
 **********************************************************/
#ifndef COUNTDOWNLATCH_H
#define COUNTDOWNLATCH_H

#include "Condition.h"

class CountDownLatch : noncopyable
{
public:
  explicit CountDownLatch(int count);

  void wait();

  void countDown();

  int getCount() const;

private:
  mutable MutexLock mutex_;//在getCount()中依然可变(要加锁)
  Condition cond_;
  int count_;

};
#endif
