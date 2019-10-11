/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-18 16:07
 * Last modified : 2019-07-18 01:01
 * Filename      : Poller.h
 * Description   : 
 **********************************************************/

#ifndef POLLER_H
#define POLLER_H
#include "IPoller.h"
#include <poll.h>

class Poller :public IPoller
{
public:
  //利用vector::data()存储::poll返回值
  typedef std::vector<struct pollfd> pollfdList;
  Poller(EventLoop *loop);
  ~Poller() = default;
  
  //IO复用,将活跃Channel放入activeChannels
  void poll(int timeout,ChannelList *activeChannels);

  //将对应channel直接从channelByFd删除，在中pollfds_与最后一个交换并删除，
  void removeChannel(Channel* channel);

  //维护和更新pollfds__   和channel_
  void updateChannel(Channel* channel);

private:
  void fill_activeChannels(int activeNum, ChannelList* activeChannels) const;

  pollfdList pollfds_;//存储::poll监听的事件,乱序,由Channel::index存储其下标

};

#endif
