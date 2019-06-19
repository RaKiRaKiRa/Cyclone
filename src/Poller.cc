/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-19 16:13
 * Last modified : 2019-06-19 16:57
 * Filename      : Poller.cc
 * Description   : 
 **********************************************************/

#include "Poller.h"

Poller::Poller(EventLoop *loop) : IPoller(loop)
{
}

//IO复用,将活跃Channel放入activeChannels
void Poller::poll(int timeout, ChannelList *activeChannels)
{
  //int poll(struct pollfd fds[], nfds_t nfds, int timeout);
  int activeNum = ::poll(pollfds_.data(), pollfds_.size(), timeout);
  int savedErr = errno;

  //若有活跃事件触发,放入activeChannels
  if(activeNum > 0)
  {
    LOG_TRACE << activeNum <<" events happened";
    fill_activeChannels(activeNum, activeChannels);
  }
  //若无事件且不报错
  else if(activeNum == 0)
  {
    LOG_TRACE << " nothing happened";
  }
  //若有错误发生
  else if(savedErr != EINTR)
  {
    errno = savedErr;
    LOG_ERROR <<"Poller::poll()"; 
  }

}
