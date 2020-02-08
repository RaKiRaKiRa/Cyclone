/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-30 20:58
 * Last modified : 2019-08-31 20:08
 * Filename      : httpStaticFile.h
 * Description   : 
 **********************************************************/

#ifndef HTTPSTATICFILE_H
#define HTTPSTATICFILE_H
#include <string>
#include <sys/stat.h>
#include <fcntl.h>
#include "base/Singleton.h"
#include "HttpType.h"

class StaticFile
{
public:
  enum fileState
  {
    OK,         // 成功
    NotFound,   // 没有找到对应文件
    ReadError,  // 文件读取错误
  };

  StaticFile(std::string file);
  ~StaticFile(){}

  // 打开文件
  bool open();

  // 将文件写入body，若没有打开则先打开
  bool writeTo(std::string& body);  

  fileState state()
  {
    return state_;
  }

  //获得文件在http相应报文中Content-Type项目应填写值
  std::string contentType() const
  {
    size_t point = file_.rfind('.');
    return Singleton<Type>::Instance().getType(file_.substr(point));

   // if(file_.find(".html") != std::string::npos)
   //   return "text/html";
   // if(file_.find(".jpg") != std::string::npos)
   //   return "image/jpeg";
   // if(file_.find(".mp4") != std::string::npos)
   //   return "video/mp4";
   // if(file_.find(".gif") != std::string::npos)
   //   return "image/gif";

   // return "text/plain";
  }

  int fd() const { return fd_; }

private:
  std::string file_;
  int fd_;
  bool closed;
  struct stat sbuf_;
  fileState state_;
  //Singleton<Type> typeMap_;
};


#endif
