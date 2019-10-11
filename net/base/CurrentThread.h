/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-05-29 17:00
 * Last modified : 2019-05-29 20:35
 * Filename      : CurrentThread.h
 * Description   : 储存线程信息 
 **********************************************************/

#ifndef CURRENT_THREAD_H 
#define CURRENT_THREAD_H

#include <stdint.h>
namespace CurrentThread
{
  extern __thread int t_cachedTid;
  extern __thread char t_tidString[32];
  extern __thread int t_tidStringLength;
  extern __thread const char* t_threadName;
  void cacheTid();

  inline int tid()
  {
    //__builtin_expect(EXP, N)。意思是：EXP==N的概率很大
    if(__builtin_expect(t_cachedTid == 0, 0))
    {
      cacheTid();
    }
    return t_cachedTid;
  }

  inline const char* tidString()
  {
    return t_tidString;
  }

  inline int tidStringLength()
  {
    return t_tidStringLength;
  }

  inline const char* threadName()
  {
    return t_threadName; 
  }
}//CurrentThread

#endif 
