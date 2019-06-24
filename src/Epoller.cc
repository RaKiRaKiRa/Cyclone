/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-19 17:12
 * Last modified : 2019-06-23 15:41
 * Filename      : Epoller.cc
 * Description   : 
 **********************************************************/
#include "Epoller.h"
#include <sys/epoll.h>


void Epoller::poll(int timeout, ChannelList* activeChannels)
{
  LOG_TRACE <<"fd totai count" << channels_.size();

  int activeNum = ::epoll_wait(epollfd_, events_.data(), static_cast<int>(events_.size()), timeout);
  int savedErr = errno;

  //有事件触发,将其加入activeChannels,由eventloop执行
  if(activeNum > 0)
  {
    LOG_TRACE<<activeNum<<" event(s) happened";
    fill_activeChannels(activeNum,activeChannels);
  }

  else if(activeNum == 0)
  {
    LOG_TRACE<<"nothing happened";
  }

  else if(savedErr != EINTR)
  {
    errno = savedErr;
    LOG_ERROR << "Epoller::poll()";
  }

}

// 活跃事件对应Channel修改revent并加入activeChannels
void Epoller::fill_activeChannels(int activeNum, ChannelList* activeChannels) const
{
/*
  struct epoll_event
  {
    uint32_t events;   
    epoll_data_t data;   
  } __attribute__ ((__packed__));

  typedef union epoll_data
  {
    void *ptr;
    int fd;
    uint32_t u32;
    uint64_t u64;
  } epoll_data_t;
*/  
for(int i = 0; i < activeNum; ++i)
  {
    Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
    //修改Channel的revent并加入activeChannels
    channel -> setRevent(events_[i].events);
    activeChannels -> push_back(channel);
  }
}


//维护和更新eventfd_   和channel_
void Epoller::updataChannel(Channel* channel)
{
  
}
