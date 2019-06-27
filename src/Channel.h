/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-15 21:54
 * Last modified : 2019-06-23 16:59
 * Filename      : Channel.h
 * Description   : 
 **********************************************************/

#ifndef CHANNEL_H
#define CHANNEL_H

#include "base/noncopyable.h"
#include <functional>

class EventLoop;

//负责一个fd的IO分发,封装其callback
//一般再封装一层,如封装成TcpConnection再使用
class Channel: noncopyable
{
public:
  typedef std::function<void()> Callback;

  Channel(EventLoop* loop, int fd);//用于绑定
  ~Channel();

  // 分发事件,根据revent_的值调用不同的回调函数
  void handleEvent();

  // 设置回调函数,通过std::move将对象的所有权转移,没有内存的搬迁和拷贝,提高效率,
  // 直接使用cb ,避免不必要的拷贝
  void setReadCallback(Callback cb)
  {
    readCallback_ = std::move(cb);
  }
  void setWriteCallback(Callback cb)
  {
    writeCallback_ = std::move(cb);
  }
  void setErrorCallback(Callback cb)
  {
    errorCallback_ = std::move(cb);
  }

  // 修改事件
  void setEvent(int ev)
  {
    //events_ = ev;
    events_ = std::move(ev);
  }
  void setRevent(int ev)
  {
    //revents_ = ev;
    revents_ = std::move(ev);
  }
  void setIndex(int idx)
  {
    index_ = std::move(idx);
  }

  // 获得相关绑定信息
  int fd() const 
  {
    return fd_;
  }
  EventLoop* loop() const
  {
    return loop_;
  }
  int event() const
  {
    return events_;
  }
  int revent() const 
  {
    return revents_;
  }
  int index() const
  {
    return index_;
  }

  // 开关读写事件,并通过EventLoop在Poller::updateChannel上更新事件列表和映射表
  void enableRead()
  {
    events_ |= k_Read;
    update();
  }
  void disableRead()
  {
    events_ &= ~k_Read;
    update();
  }
  void enableWrite()
  {
    events_ |= k_Write;
    update();
  }
  void disableWrite()
  {
    events_ &= ~k_Write;
    update();
  }
  void disableAll()
  {
    events_ = k_None;
    update();
  }
  bool isNoneEvent() const
  {
    return events_ == k_None;
  }

private:
  // 使loop_对Channel的变化进行修改
  void update();

  // 绑定事件分发器,事件描述符,监听事件和触发事件
  EventLoop *loop_;
  const int fd_;
  int       events_;
  int       revents_;
  bool      handling_;//分发事件中,保证不会被析构
  int       index_;  //在Poller中pollfds_储存位置的下标 或 Epoller中在epollfd_的状态(新加入?已存在需修改?准备删除?)
  bool      addedToLoop_;

  //事件
  static const int k_None;
  static const int k_Read;
  static const int k_Write;

  //回调函数
  Callback readCallback_;
  Callback writeCallback_;
  Callback errorCallback_;
};
#endif
