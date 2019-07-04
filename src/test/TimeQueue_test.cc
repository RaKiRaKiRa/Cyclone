/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-04 22:49
 * Last modified : 2019-07-04 22:49
 * Filename      : TimeQueue_test.cc
 * Description   : 
 **********************************************************/

#include "../EventLoop.h"

EventLoop* g_loop;

void print()
{
  printf("hahaha\n");
}

void print2()
{
  printf("Every\n");
}
void threadFunc()
{
  g_loop -> runAfter(1.0, print);
  g_loop -> runEvery(3.0, print2);
}

int main()
{
  EventLoop loop;
  g_loop = &loop;
  Thread t(threadFunc);
  t.start();
  loop.loop();
}
