#ifndef CYCLONE_THREADPOOL_H
#define CYCLONE_THREADPOOL_H
#include <Thread.h>
#include <MutexLock.h>
#include <Condition.h>
#include <vector>
#include <deque>
#include <memory>
#include <functional>

class ThreadPool: noncopyable
{
public:
    typedef  std::function<void()> Task;
    ThreadPool(int threadNum):
        threadNum_(threadNum),
        maxTaskSize(0),
        running_(false),
        mutex_(),
        isEmpty_(mutex_),
        isFull_(mutex_)
    {}

    void setThreadNum(int threadNum)
    {
        if(running_)
            return;
        threadNum_ = threadNum;
    }

    void setMaxTaskSize_(int maxTaskSize)
    {
        maxTaskSize_ = maxTaskSize;
    }

    void start();

    void stop();

    void runInThread();

    void addTask(Task task);

private:
    bool isFull()
    {
        return maxTaskSize_ > 0 && tasks_.size() >= maxTaskSize_;
    }

    bool isEmpty()
    {
        return tasks_.size() == 0;
    }

    
    std::vector<std::unique_ptr<Thread>> threads_;
    std::deque<Task> tasks_;
    int threadNum_;
    int maxTaskSize_;
    bool running_;
    MutexLock mutex_;
    Condition notEmpty_;
    Condition notFull_;

};

#endif