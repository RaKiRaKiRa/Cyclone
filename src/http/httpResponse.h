/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-27 20:10
 * Last modified : 2019-08-30 21:30
 * Filename      : httpResponse.h
 * Description   : 
 **********************************************************/

#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include "../Buffer.h"
#include <unordered_map>
#include <map>
#include <string>

class httpResponse
{
public:
  enum httpStatueCode
  {
    kUnkown,
    k200Ok               = 200, //正常处理
    k301MovedPermanently = 301, //永久性重定向，表示已经分配了新URI
    k400BadRequest       = 400, //有语法错误
    //k403Forbidden        = 403, //拒绝访问
    k404NotFound         = 404, //服务器上没有请求的资源
  };

  httpResponse() :statueCode_(kUnkown), closeConnection_(false) {}
  ~httpResponse(){  }
  
  // 设置
  void setStatueCode(httpStatueCode code)
  {
    statueCode_ = code;
  }

  void setStatueMessage(std::string mess)
  {
    statueMessage_ = std::move(mess);
  }

  void setCloseConnection(bool close)
  {
    closeConnection_ = close;
  }

  void addHeader(const std::string& key, const std::string& value)
  {
    header_[key] = value;
  }

  void setBody(std::string body)
  {
    body_ = std::move(body);
  }

  void setContentType(std::string fileType)
  {
    addHeader("Content-Type", fileType);
  }
  

  // 构造response报文
  void toBuffer(Buffer* buf);

private:
  // 各种报文信息
  httpStatueCode statueCode_;
  std::string statueMessage_;
  bool closeConnection_;

  std::unordered_map<std::string, std::string> header_;
  std::string body_;
};

#endif
