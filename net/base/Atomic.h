/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-28 20:40
 * Last modified : 2019-06-28 21:08
 * Filename      : Atomic.h
 * Description   : 
 **********************************************************/

#ifndef ATOMIC_H
#define ATOMIC_H

#include "noncopyable.h"
#include <stdint.h>

template<typename T>
class atomic : noncopyable
{
public:
  atomic():x_(0){}

  T get(){ __sync_val_compare_and_swap(&x_ , 0, 0); }
  T getAndAdd(T y){return __sync_fetch_and_add(&x_, y);}
  T addAndGet(T y){return getAndAdd(y) + y;}
  T incrementAndGet(){return addAndGet(1);}
  T decrementAndGet(){return addAndGet(-1);}
  T getAndSet(T y){return __sync_lock_test_and_set(&x_, y);}
  
  void add(T y){getAndAdd(y);}
  void increment(){incrementAndGet();}
  void decrement(){decrementAndGet();}

private:

  volatile T x_;

};

#endif
