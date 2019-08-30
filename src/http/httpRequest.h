/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-27 20:12
 * Last modified : 2019-08-30 23:11
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
  void setHeader(char *beg, char *colon, char *end)
  {
    std::string key(beg, colon);

    ++colon;
    while(colon < end && *colon == ' ')
      ++colon;
    std::string value(colon, end);
    for(int i = value.size() - 1; i >=0 ; --i)
    {
      if(value[i] != ' ')
      {
        value.resize(i + 1);
        break;
      }
    }
    header_[key] = value;
  }

  void setReceiveTime(int64_t time)
  {
    receiveTime_ = time;
  }

  //获取
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

  std::string contentType() const
  {
    if(path_.empty())
      return "";

    if(path_.find(".html") != std::string::npos)
      return "text/html";
    if(path_.find(".jpg") != std::string::npos)
      return "image/jpeg";
    if(path_.find(".mp4") != std::string::npos)
      return "video/mp4";
    if(path_.find(".gif") != std::string::npos)
      return "image/gif";

    return "text/plain";
  }

private:
  Method method_;
  Version version_;
  std::string path_;
  std::string query_;
  int64_t receiveTime_;
  std::unordered_map<std::string, std::string> header_;
};

#endif
