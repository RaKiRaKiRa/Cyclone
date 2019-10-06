/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-07 00:44
 * Last modified : 2019-10-07 00:54
 * Filename      : Connector.cc
 * Description   : 
 **********************************************************/

#include "Connector.h"
#include "EventLoop.h"
#include "Channel.h"

Connector::Connector(Eventloop* loop, sockaddr_in& addr):
  loop_(loop),
  serverAddr_(addr),
  started_(false),
  state_(kDisconnected)
{
  LOG_DEBUG << "connector [" << this <<"]";
}
