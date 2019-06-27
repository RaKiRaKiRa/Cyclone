/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-27 16:29
 * Last modified : 2019-06-27 16:29
 * Filename      : loop_test.cc
 * Description   : 
 **********************************************************/

#include <sys/timerfd.h>
#include "../EventLoop.h"
#include "../Channel.h"

EventLoop* g_loop;

void timeout()
{
  printf("Timeout!\n");
  g_loop -> quit();
}

int main()
{
  EventLoop loop;
  g_loop = &loop;

  int timer = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  Channel channel(&loop, timer);
  channel.setReadCallback(timeout);
  channel.enableRead();

  struct itimerspec howlong;
  memset(&howlong, 0, sizeof howlong);
  howlong.it_value.tv_sec = 5;
  ::timerfd_settime(timer, 0, &howlong, NULL);

  loop.loop();

  ::close(timer);
}
