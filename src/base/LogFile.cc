/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-03 22:38
 * Last modified : 2019-06-04 15:53
 * Filename      : LogFile.cc
 * Description   : 
 **********************************************************/

#include "LogFile.h"
#include <assert.h>

LogFile::LogFile(const std::string& basename,
                 off_t rollSize,
                 bool threadSafe,
                 int flushInterval,
                 int checkEveryN):
  basename_(basename),
  rollSize_(rollSize),
  flushInterval_(flushInterval),
  checkEveryN_(checkEveryN),
  count_(0),
  mutex_(threadSafe ? new MutexLock : NULL),
  startOfPeriod_(0),
  lastRoll_(0),
  lastFlush_(0)
{
  assert(basename.find('/') == std::string::npos);//保证basename中没有'/'
  rollFile();
}

LogFile::~LogFile() = default;

void LogFile::append(const char* logline, int len)
{
  if(mutex_)
  {
    MutexGuard lock(*mutex_);
    append_unlocked(logline, len);
  }
  else
  {
    append_unlocked(logline, len);
  }
}

void LogFile::flush()
{
  if(mutex_)
  {
    MutexGuard lock(*mutex_);
    file_ -> flush();
  }
  else
  {
    file_ -> flush();
  }
}

void LogFile::rollFile()
{
  time_t now = 0;//实际上time_t就是long
  std::string filename = getLogFileName(basename_, &now);
  time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

  if(now > lastRoll_)
  {
    lastRoll_ = now;
    lastFlush_ = now;
    startOfPeriod_ = start;
    //对原fp进行fclose,会同时将用户缓冲区最后剩余的数据输出到内核缓冲区
    file_.reset(new AppendFile(filename));
  }
}

void LogFile::append_unlocked(const char *logline, int len)
{
  file_ -> append(logline, len);
  //判断写入的数据长度,是否需要滚动日志
  if(file_ -> writtenBytes() > rollSize_)
  {
    //创建新fp前,对原fp进行fclose,会同时将用户缓冲区最后剩余的数据输出到内核缓冲区
    rollFile();
  }
  else//然后对时间进行判断,但每几行才判断一次
  {
    ++count_;
    //每写入几行再判断一次,减少开销
    if(count_ >= checkEveryN_)
    {
      count_ = 0;
      time_t now = time(NULL);
      time_t thisPeriod = now / kRollPerSeconds_ *kRollPerSeconds_;//当前天,用于判断是否roll
      if(thisPeriod != startOfPeriod_)
      {
        rollFile();
      }
      //是否需要flush
      else if(now - lastFlush_ > flushInterval_)
      {
        lastFlush_ = now;
        file_ -> flush();
      }
    }
  }
}

std::string LogFile::getLogFileName(const std::string &basename, time_t *now)
{
  std::string filename;
  filename.reserve(basename.size() + 64);
  filename = basename;

  char timebuf[64];
  struct tm tm;
  *now = time(NULL);
  gmtime_r(now, &tm);
  strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.log", &tm);
  filename += timebuf;
  return filename;
}
