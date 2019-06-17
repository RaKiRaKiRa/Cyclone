/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-04 17:06
 * Last modified : 2019-06-04 22:41
 * Filename      : AsyncLogging.cc
 * Description   : 
 **********************************************************/

#include "AsyncLogging.h"
#include <stdio.h>

AsyncLogging::AsyncLogging(const std::string& basename,
                           off_t rollSize,
                           int flushInterval):
  flushInterval_(flushInterval),
  running_(false),
  basename_(basename),
  rollSize_(rollSize),
  thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging"),
  latch_(0),
  mutex_(),
  cond_(mutex_),
  currentBuffer_(new Buffer),
  nextBuffer_(new Buffer),
  buffers_()
{
  currentBuffer_ -> bzero();
  nextBuffer_ -> bzero();
  buffers_.reserve(16); 
}

AsyncLogging::~AsyncLogging()
{
  if(running_)
  {
    stop();
  }
}

void AsyncLogging::append(const char *logline, int len)
{
  MutexGuard lock(mutex_);
  //若当前缓冲区放得下
  if(currentBuffer_ -> avail() > len){
    currentBuffer_ -> append(logline, len);
  }
  //否则使用预备缓冲
  else
  {
    buffers_.push_back(std::move(currentBuffer_));

    if(nextBuffer_)
    {
      currentBuffer_ = std::move(nextBuffer_);//移动而不是复制
    }
    else//若预备缓冲也被使用,申请新缓冲
    {
      currentBuffer_.reset(new Buffer);//对unique_ptr本身操作,用.
    }

    currentBuffer_ -> append(logline, len);
    cond_.notify();
  }
}

void AsyncLogging::start()
{
  running_ = true;
//  Logger::setOutput(std::bind(AsyncLogging::append, this, std::placeholders::_1 , std::placeholders::_2));
  thread_.start();  
  latch_ .wait();//等计数器降到0,即threadFun()运行后
}

void AsyncLogging::stop()
{
  running_ = false;
  cond_.notify();//通知不再等待
  thread_.join();//等待运行收尾
}

//前后端交互关键部分
void AsyncLogging::threadFunc()
{
  assert(running_ == true);
  LogFile output(basename_, rollSize_, false);// 后端控制
  BufferPtr newBuffer1(new Buffer);//前后端交换后用于补充给前端cur
  BufferPtr newBuffer2(new Buffer);//补充给next,但仅当其被使用了才替换

  BufferVector buffersToWrite;     //需要写入文件的部分,与buffers_交换

  newBuffer1 -> bzero();
  newBuffer2 -> bzero();
  buffersToWrite.reserve(16);

  while(running_)
  {
    {
      MutexGuard lock(mutex_);
      // 有待写入的缓冲或过了flushInterval_间隔则进行一次前后端交互
      if(buffers_.empty())
      {
        cond_.waitForSeconds(flushInterval_);
      }
      //移入当前缓冲区,保证buffers_有缓冲区
      buffers_.push_back(std::move(currentBuffer_));
      currentBuffer_ = std::move(newBuffer1);
      buffersToWrite.swap(buffers_);//调用的是unique<>::swap, 因为是.而不是->
      if(!nextBuffer_)
      {
        nextBuffer_ = std::move(newBuffer2);
      }
    }// unlocked

    //待写入缓冲过多则删除后面部分,因为这往往意味着有错误部分大量循环LOG
    //可以不写,但可以防止出现错误后造成的内存消耗过高和在文件io消耗大量时间
    if(buffersToWrite.size() > 25)
    {
      char buf[256];
      snprintf(buf, sizeof buf, "Dropped %d larger buffers\n", buffersToWrite.size() - 2);
      fputs(buf,stderr);
      output.append(buf, static_cast<int>(strlen(buf)));
      buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
    }

    //写入LogFile,由其写入文件,其有一个62*1024的用户缓冲区
    for(const std::unique_ptr<Buffer>& buffer : buffersToWrite)
    {
      output.append(buffer -> data(), buffer -> length());
    }
/*
    //释放多余缓存区,但似乎没有必要吧,后面有clear
    if(buffersToWrite.size() > 2)
    {
      buffersToWrite.resize(2);
    }
*/
    //补充newBuffer1 2
    if(!newBuffer1)
    {
      assert(!buffersToWrite.empty());
      newBuffer1 = std::move(buffersToWrite.back());
      buffersToWrite.pop_back();
      newBuffer1 -> reset();//cur_ = data_ 
    }

    if(!newBuffer2)
    {
      assert(!buffersToWrite.empty());
      newBuffer2 = std::move(buffersToWrite.back());
      buffersToWrite.pop_back();
      newBuffer2 -> reset();//cur_ = data_ 
    }

    buffersToWrite.clear();
    output.flush();//其实有没有都行
  }//while(running_)

  output.flush();//这个必须要有,保证退出的时候可以将log全部写入文件.
}
