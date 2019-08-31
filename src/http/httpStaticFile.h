/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-30 20:58
 * Last modified : 2019-08-31 15:43
 * Filename      : httpStaticFile.h
 * Description   : 
 **********************************************************/

#ifndef HTTPSTATICFILE_H
#define HTTPSTATICFILE_H
#include <string>
#include <sys/stat.h>
#include <fcntl.h>
#include "../base/Singleton.h"
#include "httpType.h"

class StaticFile
{
public:
  enum fileState
  {
    OK,
    NotFound,
    ReadError,
  };

  StaticFile(std::string file);
  ~StaticFile();

  bool open();

  bool writeTo(std::string& body);  

  fileState state()
  {
    return state_;
  }

  //TODO:rfind, Singleton<Type>
  std::string contentType() const
  {
    size_t point = file_.rfind('.');
    return Singleton<Type>::Instance().getType(file_.substr(point));

   // if(file_.empty())
   //   return "";

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
  struct stat sbuf_;
  fileState state_;
  //Singleton<Type> typeMap_;
};


#endif
