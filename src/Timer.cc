/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-28 21:20
 * Last modified : 2019-06-28 22:14
 * Filename      : Timer.cc
 * Description   : 
 **********************************************************/

#include "Timer.h"
#include <stdio.h>
const int64_t kMsPerS = 1000*1000;

int64_t now()
{
  struct timeval t;
  gettimeofday(&t , NULL);
  return t.tv_sec + t.tv_usec * kMsPerS;
}

std::string timeToString(int64_t tv)
{
  char buf[32];
  snprintf(buf, sizeof buf, "%lld .%lld", tv/kMsPerS, tv%kMsPerS);
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
