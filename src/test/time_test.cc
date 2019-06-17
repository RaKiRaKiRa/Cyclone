#include <stdio.h>
#include <sstream>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

  struct timeval tv;
  time_t time;
  char str_t[26] = {0};
  gettimeofday(&tv, NULL);
  time = tv.tv_sec;
  struct tm* p_time = localtime(&time);
  strftime(str_t, 26, "%Y-%m-%d %H:%M:%S ", p_time);
