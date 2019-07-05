### v0.1

- 完成了对线程控制的封装（base/Thread.h , base/Thread.cc , base/CurrentThread.h）
- 实现了互斥锁（base/MutexLock.h , base/MutexLock.cc）, 计数器（base/CountDownLatch.h , base/CountDownLatch.cc）, 条件变量（base/Condition.h , base/Condition.cc）的封装
- 实现了异步双缓冲日志系统
- 初步实现Reactor结构，可实现简单的回调绑定与使用poll/epoll (lt) 监听，测试代码与src/test/loop_test.cc



### v0.2

- 在0.1版本的基础上实现了定时器系统（base/Timer.cc , base/Timer.h , base/TimeQueue.c , base/TimeQueue.h）
  - 使用set<int64_t , Timer*>作为定时任务队列，操作的复杂度为O(logN)
  - 使用Linux提供的API **timerfd** 进行定时，将其作为一个Channel加入(e)pool进行监听
  - 提供TimeQueue::addTimer , EventLoop::runAt , EventLoop::runEvery ,EventLoop::runAfter三个接口 
- 为EventLoop提供runInLoop , queueInLoop两个接口用于执行外来功能