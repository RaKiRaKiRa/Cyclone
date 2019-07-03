/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-18 16:12
 * Last modified : 2019-06-30 17:29
 * Filename      : IPoller.cc
 * Description   : 
 **********************************************************/

#include "IPoller.h"
#include "Poller.h"
#include "Epoller.h"

//根据设置获得Epoll或Poller
IPoller* IPoller::newPoller(EventLoop* loop)
{
  if(loop -> PollerType == k_epoll)
    return new Epoller(loop);
  else
    return new Poller(loop);
}
