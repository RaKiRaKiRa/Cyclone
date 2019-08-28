/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-27 20:10
 * Last modified : 2019-08-28 17:12
 * Filename      : httpResponse.h
 * Description   : 
 **********************************************************/

#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include "../Buffer.h"

class httpResponse
{
public:

  // 构造response报文
  void toBuffer(Buffer* buf);
private:

};

#endif
