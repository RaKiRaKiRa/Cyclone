/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-30 20:57
 * Last modified : 2019-09-07 18:34
 * Filename      : httpResponse.cc
 * Description   : 
 **********************************************************/

#include "httpResponse.h"

void httpResponse::toBuffer(Buffer* buf)
{
  char line[32];
  snprintf(line, sizeof line, "HTTP/1.1 %d ", statusCode_);
  buf -> append(line);
  buf -> append(statusMessage_);
  buf -> append("\r\n");

  // 短连接
  if(closeConnection_)
  {
    buf -> append("Connection: close\r\n");
  }
  // 长连接
  else
  {
    snprintf(line, sizeof line, "Content-Length: %zd\r\n", body_.size());
    buf -> append(line);
    buf -> append("Connection: Keep-Alive\r\n");
    snprintf(line, sizeof line, "Keep-Alive: timeout=%d\r\n", keepAlive_);
    buf -> append(line);
  }

  for(std::unordered_map<std::string, std::string>::iterator it = header_.begin(); it != header_.end(); ++it)
  {
    buf -> append(it -> first);
    buf -> append(": ");
    buf -> append(it -> second);
    buf -> append("\r\n");
  }

  buf -> append("\r\n");
  buf -> append(body_);

}
