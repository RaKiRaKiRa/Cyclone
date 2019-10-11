/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-03 21:48
 * Last modified : 2019-06-03 22:38
 * Filename      : LogFile.h
 * Description   : 
 **********************************************************/

#ifndef LOGFILE_H
#define LOGFILE_H

#include "FileUtil.h"
#include "MutexLock.h"
#include <string>
#include <memory>

class AsyncLogging;
//class AppendFile;
class LogFile
{
public:
  LogFile(const std::string& basename,
          off_t rollSize,// 日志到达rollsize则生成新文件
          bool threadSafe, 
          int flushInterval = 3,//日志写入间隔
          int checkEveryN = 1024);//分割行数
  ~LogFile();

  void append(const char* logline, int len);
  void flush();
  //rolling的条件通常有两个
  //文件大小（例如每写满1GB就换下一个文件）
  //时间（例如每天零点新建一个日志文件，不论前一个文件有没有写满）
  //在append_unlocked中判断
  void rollFile();

private:
  //写入缓存区并判断roll和flush
  void append_unlocked(const char* logline, int len);
  static std::string getLogFileName(const std::string& basename, time_t *now);
  const std::string basename_;
  const off_t rollSize_;// 文件到达rollSize_则进行rollFile,即使用新的文件
  const int flushInterval_;//日志写入文件的间隔 

  const int checkEveryN_;

  int count_; //记录当前行号
  std::unique_ptr<MutexLock> mutex_;
  std::unique_ptr<AppendFile> file_;

  time_t startOfPeriod_;//新一天的时间,由rollFile()更新
  time_t lastRoll_;
  time_t lastFlush_;
  
  const static int kRollPerSeconds_ = 24*60*60;
};

#endif
