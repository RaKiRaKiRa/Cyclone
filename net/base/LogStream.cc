/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-01 23:09
 * Last modified : 2019-06-05 16:12
 * Filename      : LogStream.cc
 * Description   : 
 **********************************************************/

#include "LogStream.h"
#include <assert.h>
#include <algorithm>

//显式实例化模板,防止相同模板在多个文件进行实例化,减少开销
template class FixedBuffer<kSmallBuffer>;
template class FixedBuffer<kLargeBuffer>;

const char digits[] = "0123456789";
template<typename T>
size_t convert(char *cur, T value)
{
  T i = value;
  char *p = cur;
  do
  {
    int tmp = static_cast<int>(i % 10);
    i /= 10;
    *p++ = digits[tmp];
  }while(i != 0);

  if(value < 0)
  {
    *p++ = '-';
  }
  *p = '\0';
  std::reverse(cur, p);

  return p - cur;
}

template<typename T>
void LogStream::formatToint(T v)
{
  if(buffer_.avail() >= kMaxNumericSize)
  {
    size_t len = convert(buffer_.current(), v); 
    buffer_.add(len);
  }
}

LogStream& LogStream::operator<<(short v)
{
  *this << static_cast<int>(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned short v)
{
  *this << static_cast<unsigned int>(v);
  return *this;
}

LogStream& LogStream::operator<<(int v)
{
  formatToint(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned int v)
{
  formatToint(v);
  return *this;
}

LogStream& LogStream::operator<<(long v)
{
  formatToint(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned long v)
{
  formatToint(v);
  return *this;
}

LogStream& LogStream::operator<<(long long v)
{
  formatToint(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned long long v)
{
  formatToint(v);
  return *this;
}

LogStream& LogStream::operator<<(double v)
{
  if(buffer_.avail() >= kMaxNumericSize)
  {
    int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
    buffer_.add(len);
  }
  return *this;
}

LogStream& LogStream::operator<<(long double v)
{
  if(buffer_.avail() >= kMaxNumericSize)
  {
    int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12Lg", v);
    buffer_.add(len);
  }
  return *this;
}

LogStream& LogStream::operator<<(float v)
{
  *this<<static_cast<double>(v);
  return *this;
}
