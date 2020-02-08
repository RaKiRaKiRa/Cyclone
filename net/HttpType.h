/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-31 00:24
 * Last modified : 2019-09-01 21:14
 * Filename      : httpType.h
 * Description   : 
 **********************************************************/

#ifndef HTTPTYPE_H
#define HTTPTYPE_H
#include <unordered_map>
#include <string>

class Type
{
public:
  Type()
  {
    type_[".html"]   = "text/html";
    type_[".avi"]    = "video/x-msvideo";
    type_[".bmp"]    = "image/bmp";
    type_[".c"]      = "text/plain";
    type_[".doc"]    = "application/msword";
    type_[".gif"]    = "image/gif";
    type_[".gz"]     = "application/x-gzip";
    type_[".htm"]    = "text/html";
    type_[".ico"]    = "image/x-icon";
    type_[".jpg"]    = "image/jpeg";
    type_[".png"]    = "image/png";
    type_[".txt"]    = "text/plain";
    type_[".mp3"]    = "audio/mp3";
    type_["default"] = "text/html";
    type_[".js"]     = "application/javascript";
    type_[".css"]    = "text/css";
  }

  std::string getType(std::string key)
  {
    std::unordered_map<std::string, std::string>::iterator it = type_.find(key);
    if(it != type_.end())
      return it -> second;
    return "text/plain";
  }


private:
  std::unordered_map<std::string, std::string> type_;
};


#endif
