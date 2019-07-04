/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-28 21:20
 * Last modified : 2019-06-30 21:29
 * Filename      : Timer.cc
 * Description   : 
 **********************************************************/

#include "Timer.h"
#include <stdio.h>

const int64_t kMsPerS = 1000*1000;
atomic<int64_t> Timer::timerCreated_;

void Timer::restart(int64_t now)
{
  if(static_cast<bool>(repeat_))
  {
    expiration_ = nextEpiration(now);
  }
  else
  {
    expiration_ = 0;
  }
}
