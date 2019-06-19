/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-18 16:07
 * Last modified : 2019-06-18 22:51
 * Filename      : Epoller.h
 * Description   : 
 **********************************************************/

#ifndef EPOLLER_H
#define EPOLLER_H

#include "IPoller.h"
#include <sys/epoll.h>

class Epoller :public IPoller
{
public:
  //通过data() 实现对epoll_wait返回值的存储
  typedef std::vector<struct epoll_event> eventList;

  Epoller(EventLoop* loop);
  
  ~Epoller();
  
  //IO复用,将活跃Channel放入activeChannels
  void poll(int timeout, ChannelList* activeChannels);
  
  void removeChannel(Channel* channel);

  void updataChannel(Channel* channel);

  void fill_activeChannels(int activeNum, ChannelList* activeChannels);
private:
  const int eventList_InitialSize = 16;


  int epollfd_;
  //存储epoll_wait返回值
  eventList events_;

};

#endif
