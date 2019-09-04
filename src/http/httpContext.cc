/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-28 12:21
 * Last modified : 2019-09-04 22:22
 * Filename      : httpContext.cc
 * Description   : 
 **********************************************************/

#include "httpContext.h"

httpContext::httpContext():
  state_(kRequestLine)
{}

bool httpContext::parseRequest(Buffer* buf)
{
  bool hasMore = true, ok = true;
  int length = 0;
  // 开始分析
  while(hasMore)
  {
    // 分析请求行
    if(state_ == kRequestLine)
    {
      const char* crlf = buf -> findCRLF();
      // 找到一行
      if(crlf)
      {
        // 找到一行内容，进行分析判断
        ok = parseRequestLine(buf -> peek(), crlf);
        
        if(ok)// 请求行分析成功
        {
          buf -> retrieveUntil(crlf + 2);
          state_ = kRequestHeader; 
        }     
        else// 分析失败，退出分析
        {
          hasMore = false;
        }
      }// <<end crlf
      else // 没找到一行，退出分析
      {
        hasMore = false;
      }
    } // <<end state_ == kRequestLine
    // 分析头部字段
    else if(state_ == kRequestHeader)
    {
      const char* crlf = buf -> findCRLF();
      // 找到一行
      if(crlf)
      {
        const char* colon = std::find(buf -> peek(), crlf, ':');
        if(colon != crlf)
        {
          request_.addHeader(buf -> peek(), colon, crlf);
        }
        // 没有找到冒号，根据Content-Length判断是否有报文体
        else
        {
          std::string contentLength = request_.header("Content-Length");
          if(contentLength.size() == 0 || contentLength == "0")
          {
            state_ = kComplete;
            hasMore = false;
          }
          else
          {
            length = stoi(contentLength);
            state_ = kRequestBody;
          }
        }
        buf -> retrieveUntil(crlf + 2);
      } // <<end crlf
      // 无行可读，
      else
      {
        hasMore = false;
      }
    } // <<end state_ == kRequestHeader
    // 分析请求体
    else if(state_ == kRequestBody)
    {
      LOG_DEBUG << "buf -> readableBytes() = " << buf ->readableBytes();
      LOG_DEBUG << "Content-Length = " << length;
      if(buf -> readableBytes() >= length)
      {
        request_.setBody(buf -> peek(), buf -> peek() + length);
        hasMore = false;
      }
      else
      {
        hasMore = false;
      }
    } // <<end state_ == kRequestBody
  } // <<end while(hasMore)

  return ok;
}

bool httpContext::parseRequestLine(const char* beg, const char* end)
{

}


