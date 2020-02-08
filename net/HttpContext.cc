/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-28 12:21
 * Last modified : 2020-02-08 15:56
 * Filename      : httpContext.cc
 * Description   : 
 **********************************************************/

#include "HttpContext.h"

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

// FIXME : 有bug
bool httpContext::parseRequestLine(const char* beg, const char* end)
{
  // 例如：GET /web/index.html?a=b HTTP/1.1
  // 分别匹配Method Path （可能有的Query） 和 Version
  bool success = false;
  const char* begin = beg;
  const char* space = std::find(begin, end, ' ');

  // 先匹配Method
  if(space != end && request_.setMethod(begin, space))
  {
    begin = space + 1;
    space = std::find(begin, end, ' ');
    // 在匹配Path
    if(space != end)
    {
      // 是否有query
      const char* query = std::find(begin, space, '?');
      if(query != space)
      {
        request_.setQuery(query, space);
        request_.setPath(begin, query);
      }
      else
      {
        request_.setPath(begin, space);
      }

      // 再匹配Version
      begin = space + 1;
      success = (end - begin == 8) && std::equal(begin, end - 1, "HTTP/1.");
      if(success)
      {
        if(*(end - 1) == '1')
        {
          request_.setVersion(httpRequest::kHttp11);
        }
        else if(*(end - 1) == '0')
        {
          request_.setVersion(httpRequest::kHttp10);
        }
        else
        {
          success = false;
        }
      } // <<end if(success)
    } // <<end if(space != end)
  } // <<end if(space != end && request_.setMethod(begin, space))
  return success;
}


