/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-05-29 20:42
 * Last modified : 2019-05-29 20:42
 * Filename      : CountDownLatch.cc
 * Description   : 
 **********************************************************/

#include "CountDownLatch.h"

CountDownLatch::CountDownLatch(int count):
  mutex_(),
  cond_(mutex_),
  count_(count)
{
}

void CountDownLatch::wait()
{
  MutexGuard lock(mutex_);
  while(count_ > 0)
  {
    cond_.wait();
  }
}

void CountDownLatch::countDown(){
  MutexGuard lock(mutex_);
  --count_;
  if(count_ == 0)
    cond_.notifyAll();
}

int CountDownLatch::getCount() const
{
  MutexGuard lock(mutex_);
  return count_;
}
