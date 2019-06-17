/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-02 20:16
 * Last modified : 2019-06-02 22:54
 * Filename      : Logging.h
 * Description   : 
 **********************************************************/

#ifndef LOGGING_H
#define LOGGING_H

#include "LogStream.h"
#include "CurrentThread.h"
#include <string>
#define LOG Logger(__FILE__, __LINE__, __func__).stream()
#define LOG_TRACE Logger(__FILE__, __LINE__, __func__, Logger::TRACE).stream()
#define LOG_DEBUG Logger(__FILE__, __LINE__, __func__, Logger::DEBUG).stream()
#define LOG_INFO  Logger(__FILE__, __LINE__, __func__, Logger::INFO ).stream()
#define LOG_WARN  Logger(__FILE__, __LINE__, __func__, Logger::WARN ).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, __func__, Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, __func__, Logger::FATAL).stream()
class AsyncLogging;

class Logger
{
public:
  enum LogLevel
  {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    NUM_LOG_LEVELS,
  };

  typedef void (*OutputFunc)(const char* msg, int len);
  typedef void (*FlushFunc)();
  Logger(const char *file, int line, const char *func, LogLevel level = INFO);
  ~Logger();
  LogStream& stream()
  {
    return impl_.stream_;
  }
  static void setOutput(OutputFunc);
  static void setFlush(FlushFunc);
private:
  class Impl
  {
  public:
    Impl(const char *file, int line, LogLevel level);

    void formatTime();
    LogStream stream_;
    const char *file_;
    int line_;
    LogLevel level_;
  };
  Impl impl_;
  //static std::string logFileName;
};


#endif
