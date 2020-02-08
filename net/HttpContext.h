/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-27 20:09
 * Last modified : 2020-02-08 15:57
 * Filename      : httpContext.h
 * Description   : 
 **********************************************************/
#ifndef HTTPCONTEXT_H
#define HTTPCONTEXT_H
#include "HttpRequest.h"
#include "Httpserver.h"

class httpContext
{
public:
  enum parseState
  {
    kRequestLine,    // 正在解析请求行
    kRequestHeader,  // 正在解析头部字段
    kRequestBody,    // 正在解析请求体
    kComplete,       // 解析结束
  };
  
  httpContext();
  ~httpContext(){};

  // 解析报文
  bool parseRequest(Buffer* buf);

  bool getAll()
  {
    return state_ == kComplete;
  }

  void reset()
  {
    state_ = kRequestLine;
    httpRequest tmp;
    std::swap(tmp, request_);
  }

  httpRequest& Request()
  {
    return request_;
  }
  const httpRequest& Request() const
  {
    return request_;
  }

private:
  // 解析报文请求行
  bool parseRequestLine(const char* begin, const char* end);

  parseState state_;

  httpRequest request_;
};

#endif
