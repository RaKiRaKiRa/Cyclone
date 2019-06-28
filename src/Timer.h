/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-28 20:39
 * Last modified : 2019-06-28 22:17
 * Filename      : Timer.h
 * Description   : 
 **********************************************************/

#ifndef TIMER_H
#define TIMER_H
#include "base/Atomic.h"
#include <string>
#include <sys/time.h>

int64_t now();
std::string timeToString(int64_t t);
std::string timeToFormattedString(int64_t t, bool showMs = true);

class Timer:noncopyable
{
public:

private:

  int64_t expiration_;
  static atomic<int64_t> timerCreated;
};


#endif

