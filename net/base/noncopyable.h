/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-05-29 16:46
 * Last modified : 2019-05-29 16:46
 * Filename      : noncopyable.h
 * Description   : 
 **********************************************************/
#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

class noncopyable
{
public:
  noncopyable(const noncopyable&) = delete;
  void operator=(const noncopyable&) = delete;
protected:
  noncopyable() = default;
  ~noncopyable() = default;
};

#endif
