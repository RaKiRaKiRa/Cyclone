/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-15 21:54
 * Last modified : 2019-06-16 17:34
 * Filename      : Channel.h
 * Description   : 
 **********************************************************/

#ifndef CHANNEL_H
#define CHANNEL_H

#include "base/noncopyable.h"
#include <functional>

class EventLoop;

//负责一个fd的IO分发,封装其callback
class Channel: noncopyable
{
public:
  typedef std::function<void()> EventCallback;

  Channel(EventLoop* loop, int fd_);//用于绑定

  void handleEvent();
private:
  //绑定
  EventLoop *loop_;
  const int fd_;
  //绑定事件与返回事件
  int       events_;
  int       revents_;

  EventCallback readCallback_;
  EventCallback writeCallback_;
  EventCallback errorCallback_;
};
#endif
