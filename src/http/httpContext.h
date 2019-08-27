/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-27 20:09
 * Last modified : 2019-08-27 20:29
 * Filename      : httpContext.h
 * Description   : 
 **********************************************************/
#ifndef HTTPCONTEXT_H
#define HTTPCONTEXT_H
#include "httpRequest.h"
#include "httpserver.h"

class httpContext
{
public:
  bool parseRequest(Buffer* buf);

  bool getAll();

  httpRequest& getRequest() const;
private:
  httpRequest request_;
};

#endif
