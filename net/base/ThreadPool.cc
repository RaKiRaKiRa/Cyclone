#include <ThreadPool.h>

void ThreadPool::start()
{
    running_ = true;
    threads_.reserve(numThreads);
    for(int i = 0; i < threadNum_; ++i)
    {
        threads_.emplace_back(new Thread(
            std::bind(&ThreadPool::runInThread, this, std::to_string(i + 1))));
        thread_[i]->start();
    }
}

void ThreadPool::stop()
{
    running_ = false;
    notEmpty_.notifyAll();
    for(int i = 0; i < threadNum_; ++i)
        threads_[i].join();
}

void ThreadPool::runInThread()
{
    while(running_)
    {
        MutexGuard lock(mutex_);
        while(running_ && isEmpty())
        {
            notEmpty_.wait;
        }
        if(running_)
        {
            Task task = tasks_.front();
            tasks.pop_front();
            notFull_.notifyAll();
            task();
        }
    }
}

void ThreadPool::addTask(Task task)
{
    MutexGuard lock(mutex_);
    while(isFull())
    {
        notFull_.wait();
    }
    tasks_.push_back(task);
    notEmpty_.notifyAll();
}