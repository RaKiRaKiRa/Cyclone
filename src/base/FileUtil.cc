/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-03 15:36
 * Last modified : 2019-06-03 21:48
 * Filename      : FileUtil.cc
 * Description   : 
 **********************************************************/

#include "FileUtil.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

__thread char t_errnobuf[512];

AppendFile::AppendFile(std::string filename):
  fp_(fopen(filename.c_str(), "ae"))
{
  assert(fp_);
  setbuffer(fp_, buffer_, sizeof buffer_);
}

AppendFile::~AppendFile()
{
  fclose(fp_);
}

void AppendFile::flush()
{
  fflush(fp_);
}

void AppendFile::append(const char* logline, const size_t len)
{
  size_t n = write(logline, len);
  size_t remain = len - n;
  while(remain > 0)
  {
    size_t x = write(logline, len);
    if(x == 0)
    {
      int err = ferror(fp_);
      if(err)
      {
        fprintf(stderr, "AppendFile::append() failed %s \n", strerror_r(err, t_errnobuf, sizeof t_errnobuf));
      }
      break;
    }
    n += x;
    remain = len - n;
  }
  writtenBytes_ += len;
}

size_t AppendFile::write(const char* logline, const size_t len)
{
  //为了快速，使用unlocked(无锁)的fwrite函数。平时我们使用的C语言IO函数，都是线程安全的，
  //为了做到线程安全，会在函数的内部加锁。这会拖慢速度。而对于这个类。可以保证，从
  //始到终只有一个线程能访问，所以无需进行加锁操作。
  //size_t fwrite(const void* buffer, size_t size, size_t count, FILE* stream);
  //size要写入内容的单字节数；
  //count要进行写入size字节的数据项的个数；
  return fwrite_unlocked(logline, 1, len, fp_);
}
