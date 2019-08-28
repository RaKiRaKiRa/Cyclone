/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-28 12:21
 * Last modified : 2019-08-28 15:59
 * Filename      : httpContext.cc
 * Description   : 
 **********************************************************/

#include "httpContext.h"

httpContext::httpContext():
  state_(kRequestLine)
{}

bool httpContext::parseRequest(Buffer* buf)
{

}

bool httpContext::parseRequestLine(const char* beg, const char* end)
{

}


