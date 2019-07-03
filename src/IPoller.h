/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-18 15:40
 * Last modified : 2019-06-30 17:29
 * Filename      : IPoller.h
 * Description   : 
 **********************************************************/

#ifndef IPOLLER_H
#define IPOLLER_H

#include "EventLoop.h"
#include "Channel.h"
#include <vector>
#include <map>


//Poller和EPoller的抽象基类
//通过newPoller获得派生类
class IPoller : noncopyable
{
public:
  // Poller不拥有Channel,只是保存其指针进行调用
  // Poller中存储Channel*在ChannelMap中, 并以fd为key
  // List是EventLoop中的数据结构
  typedef std::vector<Channel*> ChannelList;
  typedef std::map<int, Channel*> ChannelMap;

  IPoller(EventLoop* loop) : loop_(loop){};
  ~IPoller() = default;

  //IO复用,将活跃Channel放入activeChannels
  virtual void poll(int timeout, ChannelList *activeChannels) = 0;

  //增删查,全是虚函数,因为分epoll和poll两种情况
  

  virtual void removeChannel(Channel* channel) = 0;

  //维护和更新pollfds_/eventfd_   和channel_
  virtual void updateChannel(Channel* channel) = 0;


  bool hasChannel(Channel* channel) const
  {
    ChannelMap::const_iterator it = channelsByFd_.find(channel -> fd());
    return it != channelsByFd_.end() && it -> second == channel;
  }

  void assertInLoop() const
  {
    loop_ -> assertInLoopThread();
  }
  // 返回EventLoop指定的POLLER
  static IPoller* newPoller(EventLoop* loop);
private:
  EventLoop *loop_;

protected:
  virtual void fill_activeChannels(int activeNum, ChannelList* activeChannels) const = 0;
  //Channel::fd : Channel*
  ChannelMap channelsByFd_;
  
};

#endif
