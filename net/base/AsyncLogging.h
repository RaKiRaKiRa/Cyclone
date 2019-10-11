/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-04 16:36
 * Last modified : 2019-06-04 17:06
 * Filename      : AsyncLogging.h
 * Description   : 
 **********************************************************/

#ifndef ASYNCLOGGING_H
#define ASYNCLOGGING_H

#include "LogFile.h"
#include "Logging.h"
#include "CountDownLatch.h"
#include "Thread.h"
#include <assert.h>
#include <string>
#include <vector>
#include <atomic>

class AsyncLogging :noncopyable
{
public:
  AsyncLogging(const std::string& basename, off_t rollSize, int flushInterval = 3);
  ~AsyncLogging();
  void append(const char* logline, int len);

  void start();
  void stop();

private:
  void threadFunc();

  typedef FixedBuffer<kLargeBuffer> Buffer;
  typedef std::vector<std::unique_ptr<Buffer> > BufferVector;
  typedef BufferVector::value_type BufferPtr;//即unique_ptr<Buffer>

  const int flushInterval_;//前后端buffer交换的超时时间,默认与LogFile的flush时间数值上保持相同,但不一定要相同
  std::atomic<bool> running_;
  const std::string basename_;
  const off_t rollSize_;
  Thread thread_;
  
  CountDownLatch latch_;
  MutexLock mutex_;
  Condition cond_;

  BufferPtr currentBuffer_;//当前缓冲
  BufferPtr nextBuffer_;   //预备缓冲
  BufferVector buffers_;   //待写入文件的已经填满的缓冲的指针
};

#endif
