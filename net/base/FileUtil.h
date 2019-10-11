/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-03 15:07
 * Last modified : 2019-06-03 15:36
 * Filename      : FileUtil.h
 * Description   : 
 **********************************************************/

#ifndef FILEUTIL_H
#define FILEUTIL_H

#include "noncopyable.h"
#include <string>
#include <sys/types.h>

class AppendFile : noncopyable
{
public:
  explicit AppendFile(std::string);
  ~AppendFile();
  void append(const char *logline, const size_t len);
  void flush();
  off_t writtenBytes() const
  {
    return writtenBytes_;
  }
private:
  FILE* fp_;
  char buffer_[64*1024];
  off_t writtenBytes_;
  size_t write(const char *logline, size_t len);

};

#endif
