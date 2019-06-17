/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-02 20:42
 * Last modified : 2019-06-02 22:53
 * Filename      : Logging.cc
 * Description   : 
 **********************************************************/
#include "Logging.h"
#include <stdio.h>
#include <sstream>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

const char* LogLevelName[Logger::NUM_LOG_LEVELS] = 
{
  "TRACE ",
  "DEBUG ",
  "INFO  ",
  "WARN  ",
  "ERROR ",
  "FATAL ",
};

void defaultOutput(const char* msg, int len)
{
  size_t n = fwrite(msg, 1, len, stdout);
  (void)n;
}

void defaultFlush()
{
  fflush(stdout);
}

Logger::OutputFunc g_output = defaultOutput ;
Logger::FlushFunc g_flush = defaultFlush;

void Logger::setOutput(OutputFunc out)
{
  g_output = out;
}

void Logger::setFlush(FlushFunc flush)
{
  g_flush = flush;
}

Logger::Impl::Impl(const char *file, int line, LogLevel level):
  file_(file),
  line_(line),
  level_(level)
{
  formatTime();
  CurrentThread::tid();
  stream_ << CurrentThread::tidString();
  stream_ << LogLevelName[level_];
}

void Logger::Impl::formatTime()
{
  struct timeval tv;
  time_t time;
  char str_t[26] = {0};
  gettimeofday(&tv, NULL);
  time = tv.tv_sec;
  struct tm* p_time = localtime(&time);
  strftime(str_t, 26, "%Y-%m-%d %H:%M:%S ", p_time);
  stream_ << str_t;
}

Logger::Logger(const char *file, int line, const char *func, LogLevel level):
  impl_(file, line, level)
{
  impl_.stream_ << func <<' ';
}

Logger::~Logger()
{
  impl_.stream_ << "--" << impl_.file_ << ':' << impl_.line_ << '\n';
  const LogStream::Buffer& buf(stream().buffer());
  g_output(buf.data(),buf.length());
}
