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

Epoller::Epoller(EventLoop* loop):
  IPoller(loop),
  //int epoll_create(int size);    size并没有实际作用
  //int epoll_create1(int flags);  flag == 0时与上函数相同,EPOLL_CLOEXEC标志与open 时的O_CLOEXEC 标志类似，即进程被替换时会关闭打开的文件描述符
  //                                                       使用完epoll后，必须调用close()关闭，否则可能导致fd被耗尽
  epollfd_(epoll_create1(EPOLL_CLOEXEC)),
  events_(eventList_InitialSize)
{
  if(epollfd_ < 0)
    LOG_ERROR << "Epoller::Epoller()";
}


void Epoller::poll(int timeout, ChannelList* activeChannels)
{
  LOG_TRACE <<"fd totai count" << channelsByFd_.size();

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

//Epoller中的状态储存在对应Channel的index_中
const int kNew     = -1; //当前Channel是新的，并且需要添加进epoll文件描述符中
const int kAdded   =  1; //当前Channel存在且非None,已加入epollfd
const int kDeleted =  2; //当前Channel之前存在且处于NoneEvent，现在需要添加进epoll文件描述符中

//维护和更新eventfd_   和channel_
void Epoller::updateChannel(Channel* channel)
{
  assertInLoop();
  int state = channel -> index();
  int fd    = channel -> fd();
  if(state == kNew || state == kDeleted)//若是新channel或处于NoneEvent,即没有加入epollfd
  {
    if(state == kNew)//未加入,则加入
    {
      assert(channelsByFd_.find(fd) == channelsByFd_.end());
      channelsByFd_[fd] = channel;
    }
    else//已加入
    {
      assert(channelsByFd_.find(fd) != channelsByFd_.end());
      assert(channelsByFd_[fd] == channel);
    }
    
    channel -> setIndex(kAdded);
    update(EPOLL_CTL_ADD, channel);
  }
  else
  {
    //int fd = channel -> fd();

    if(channel -> isNoneEvent())//无监听事件,不关注,设为kDeleted
    {
      update(EPOLL_CTL_DEL, channel);
      channel -> setIndex(kDeleted);
    }
    else
      update(EPOLL_CTL_MOD, channel);
  }
}


void Epoller::update(int op, Channel* channel)
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
  struct epoll_event ev;
  memset(&ev, 0, sizeof ev);

  ev.events = channel -> event();
  ev.data.ptr = channel;
  int fd = channel -> fd();

  if(epoll_ctl(epollfd_, op, fd, &ev) < 0)
  {
    LOG_ERROR << "Epoller::update() fd = "<< fd << "operator = " << op; 
  }
}


void removeChannel(Channel* channel)
{

}
