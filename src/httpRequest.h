/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-27 20:12
 * Last modified : 2019-09-04 22:18
 * Filename      : httpRequest.h
 * Description   : 
 **********************************************************/

#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <string>
#include <unordered_map>
#include <map>

class httpRequest
{
public:

  enum Method
  {
    kGet,
    kPost,
    kUnkownMethod,
  };

  enum Version
  {
    kHttp10,
    kHttp11,
    kUnkownVersion,
  };

  httpRequest() : method_(kUnkownMethod), version_(kUnkownVersion) {}
  ~httpRequest(){}
  //设置
  bool setMethod(const char *beg, const char *end)
  {
    std::string tmp(beg, end);
    if(tmp == "GET")
      method_ = kGet;
    else if(tmp == "POST")
      method_ = kPost;
    else
      method_ = kUnkownMethod;
    return method_ != kUnkownMethod;
  }

  void setVersion(const Version ver)
  {
    version_ = ver;
  }

  void setPath(const char *beg, const char *end)
  {
    path_.assign(beg, end);
  }

  void setQuery(const char *beg, const char *end)
  {
    query_.assign(beg, end);
  }

  //TODO
  void addHeader(const char *beg, const char *colon, const char *end)
  {
    std::string key(beg, colon);

    ++colon;
    while(colon < end && *colon == ' ')
      ++colon;
    std::string value(colon, end);
    int i = 0;
    for(i = value.size() - 1; i >=0 ; --i)
    {
      if(value[i] != ' ')
      {
        value.resize(i + 1);
        break;
      }
    }
    if(i == 0 && value[i] == ' ')
      return ;

    header_[key] = value;
  }

  void setReceiveTime(int64_t time)
  {
    receiveTime_ = time;
  }
  void setBody(const char *beg, const char *end)
  {
    body_.assign(beg, end);
  }

  //获取
  std::string body() const
  {
    return body_;
  }
  Version version() const 
  {
    return version_;
  }
  Method method() const
  {
    return method_;
  }
  std::string methodToString() const
  {
    if(method_ == kGet)
      return "GET";
    else if(method_ == kPost)
      return "POST";
    else
      return "UNKOWN";
  }
  std::string path() const
  {
    return path_;
  }
  std::string query() const
  {
    return query_;
  }
  std::string header(const std::string& s)
  {
   std::unordered_map<std::string, std::string>::iterator it = header_.find(s);
   if(it != header_.end())
     return it -> second;
   else
     return "";
  }

  int64_t  receiveTime()
  {
    return receiveTime_;
  }
private:
  Method method_;
  Version version_;
  std::string path_;
  std::string query_;
  std::string body_;
  int64_t receiveTime_;
  std::unordered_map<std::string, std::string> header_;
};

#endif
