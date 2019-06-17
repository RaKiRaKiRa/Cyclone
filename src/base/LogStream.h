/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-01 21:16
 * Last modified : 2019-06-02 00:04
 * Filename      : LogStream.h
 * Description   : 
 **********************************************************/

#ifndef LOGSTREAM_H
#define LOGSTREAM_H

#include "noncopyable.h"
#include <string>
#include <string.h>
#include <assert.h>


class AsyncLogging;
const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000*1000;


template<int SIZE>
class FixedBuffer : noncopyable
{
public:
  FixedBuffer():
    cur_(data_)
  {}

  ~FixedBuffer()
  {}

  void append(const char* buf, size_t len)
  {
    if(avail() > static_cast<int>(len))
    {
      memcpy(cur_, buf, len);
      cur_ += len;
    }
  }

  const char *data() const 
  {
    return data_;
  }

  int avail() const
  {
    return static_cast<int>(end() - data_);
  }

  void add(size_t len)
  {
    cur_ += len;
  }

  void reset()
  {
    cur_ = data_;
  }

  void bzero()
  {
    memset(data_, 0, sizeof(data_));
  }

  char* current() const 
  {
    return cur_;
  }

  int length() const
  {
    return static_cast<int>(cur_ - data_);
  }

  std::string toString()
  {
    return std::string(data_, length());
  }
private:
  const char* end() const
  {
    return data_ + sizeof(data_);
  }
  char data_[SIZE];
  char *cur_;

};

class LogStream: noncopyable
{
public:
  typedef FixedBuffer<kSmallBuffer> Buffer;

  LogStream& operator<<(bool v)
  {
    buffer_.append(v ? "1": "0", 1);
    return *this;
  }

  LogStream& operator<<(char v)
  {
    buffer_.append(&v, 1);
    return *this;
  }

  LogStream& operator<<(const char* str)
  {
    if(str)
      buffer_.append(str, strlen(str));
    else
      buffer_.append("(null)", 6);
    return *this;
  }

  LogStream& operator<<(const unsigned char* str)
  {
    //操作符修改了操作数类型,但仅仅是重新解释了给出的对象的比特模型而没有进行二进制转换
    return operator<<(reinterpret_cast<const char*>(str));
  }

  LogStream& operator<<(const std::string& str)
  {
    buffer_.append(str.c_str(), str.size());
    return *this;
  }

  LogStream& operator<<(short);
  LogStream& operator<<(unsigned short);
  LogStream& operator<<(int);
  LogStream& operator<<(unsigned int);
  LogStream& operator<<(long);
  LogStream& operator<<(unsigned long);
  LogStream& operator<<(long long);
  LogStream& operator<<(unsigned long long);

//  LogStream& operator<<(const void*);
  LogStream& operator<<(float);
  LogStream& operator<<(double);
  LogStream& operator<<(long double);
  
  void append(const char* data, size_t len)
  {
    buffer_.append(data, len);
  }

  const Buffer& buffer()
  {
    return buffer_;
  }

  void resetBuffer()
  {
    buffer_.reset();
  }

private:
  template<typename T>
    void formatToint(T);
  Buffer buffer_;
  static const int kMaxNumericSize = 32;
};

#endif
