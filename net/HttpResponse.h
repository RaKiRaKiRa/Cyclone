/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-27 20:10
 * Last modified : 2020-02-08 16:02
 * Filename      : httpResponse.h
 * Description   : 
 **********************************************************/

#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include "Buffer.h"
#include <unordered_map>
#include <map>
#include <string>
#include "HttpStaticFile.h"

class httpResponse
{
public:
  enum httpStatusCode
  {
    kUnkown,
    k200Ok               = 200, //正常处理
    k301MovedPermanently = 301, //永久性重定向，表示已经分配了新URI
    k400BadRequest       = 400, //有语法错误
    //k403Forbidden        = 403, //拒绝访问
    k404NotFound         = 404, //服务器上没有请求的资源
  };

  explicit httpResponse(bool close, int keepAlive) :
    statusCode_(kUnkown), 
    closeConnection_(close),
    keepAlive_(keepAlive)
  {}

  ~httpResponse(){  }
  
  // 设置
  
  void setStatusCode(httpStatusCode code)
  {
    statusCode_ = code;
  }

  void setStatusMessage(std::string mess)
  {
    statusMessage_ = std::move(mess);
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

  bool setBody(StaticFile& staticFile)
  {
    return staticFile.writeTo(body_);
  }

  void setContentType(std::string fileType)
  {
    addHeader("Content-Type", fileType);
  }
  
  // 找不到文件，准备发送404报文
  void setNotFound()
  {
    header_.clear();
    setStatusCode(k404NotFound);
    setStatusMessage("Not Found");
    setContentType("text/html");
    addHeader("Server", "Cyclone");
    setBody("<html><head><title>This is title</title></head>"
      "<body><h1>404 Not Found</h1></html>");
    setCloseConnection(true);
  }

  // 构造response报文
  void toBuffer(Buffer* buf);

  bool closeConnection() const
  {
    return closeConnection_;
  }

private:
  // 各种报文信息
  httpStatusCode statusCode_;
  std::string statusMessage_;
  bool closeConnection_;
  int keepAlive_;
  std::unordered_map<std::string, std::string> header_;
  std::string body_;
};

#endif
