/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-09-09 18:50
 * Last modified : 2019-09-09 19:26
 * Filename      : Daemon.cc
 * Description   : 
 **********************************************************/

#include "Daemon.h"
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>


void Daemon()
{
  // 重设文件权限掩码
  // 继承来的文件模式创建屏蔽字可能会拒绝设置某种权限，而守护进程需要创建的文件要具有读写权限。
  umask(0);

  // 使父进程忽略子进程结束信号
  // 守护进程中父进程将自己结束，使子进程挂在init下成为僵尸进程
  signal(SIGCHLD, SIG_IGN);

  //1）在父进程中，fork返回新创建子进程的进程ID；
  //2）在子进程中，fork返回0；
  //3）如果出现错误，fork返回一个负值；
  pid_t pid = fork();
  if(pid < 0)
  {
    std::cout << "fork ERROR" << std::endl;
    exit(-1);
  }
  //父进程退出，子进程独立运行
  else if(pid > 0)
  {
    exit(0);
  }

  //之前parent和child运行在同一个session里,parent是会话（session）的领头进程,
  //parent进程作为会话的领头进程，如果exit结束执行的话，那么子进程会成为孤儿进程，并被init收养。
  //执行setsid()之后,child将重新获得一个新的会话(session)id。
  //这时parent退出之后,将不会影响到child了。
  // 作用：
  //1. 让子进程摆脱原会话的控制。
  //2. 让子进程摆脱原进程组的控制。
  //3. 让子进程摆脱原控制终端的控制。
  setsid();

  // 基于安全性，一般会进行两次fork
  // 第一次fork:是为了调用setsid函数，将子进程自成一个独立会话，成为一个进程组，且不受控制终端的控制。
  // 第二次fork:由于当前的子进程为独立会话且为会话组长，独立的进程组，
  //            不受控制终端的控制且可打开控制终端，则需再进行一次fork，之后会话sid与id不同，其便不可以打开控制终端。
  pid_t pid2 = fork();
  if(pid2 < 0)
  {
    std::cout << "fork ERROR" << std::endl;
    exit(-1);
  }
  //父进程退出，子进程独立运行
  else if(pid2 > 0)
  {
    exit(0);
  }

  // 关闭不需要的与终端相关的文件描述符，或者重定向到/dev/null中
  int fd = open("/dev/null", O_RDWR, 0);
  if(fd != -1)
  {
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
  }
  if(fd > 2)
    close(fd);
}
