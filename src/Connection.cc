/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-18 15:26
 * Last modified : 2019-10-09 23:48
 * Filename      : Connection.cc
 * Description   : 
 **********************************************************/

#include "Connection.h"
#include "Server.h"
#include "EventLoop.h"
#include "Channel.h"

// 若读完不操作，则直接清空
void defaultMessCallback(const ConnectionPtr& ptr, Buffer* buf)
{
  buf -> clear();
}

// 若连接玩不操作则仅仅LOG
void defaultConnCallback(const ConnectionPtr& conn)
{
  LOG_TRACE << toIpPort(conn ->local()) << " -> " << toIpPort(conn -> peer()) << " " << (conn -> connected()?"Up":"Down"); 
}


//TODO:未保证Connection安全，应该在Channel中建立一个对应Connection的weak_ptr，在进行handleEvent时创建一个lock对象，结束时可以使其自动销毁

Connection::Connection(EventLoop *loop, std::string name, sockaddr_in local, sockaddr_in peer, int sockfd):
  loop_(loop),
  name_(name),
  state_(kConnecting),
  peer_(peer),
  local_(local),
  socket_(new Socket(sockfd)),
  channel_(new Channel(loop, sockfd)),
  anyPtr(NULL)
{
  channel_ -> setReadCallback(std::bind(&Connection::handleRead,this));
  channel_ -> setWriteCallback(std::bind(&Connection::handleWrite,this));
  channel_ -> setCloseCallback(std::bind(&Connection::handleClose,this));
  channel_ -> setErrorCallback(std::bind(&Connection::handleError,this));

  LOG_DEBUG << "Connection [" << name_ <<"] at " <<this <<" fd = "<<sockfd;
}


Connection::~Connection()
{
  LOG_DEBUG << "Connection [" << name_ <<"] at " <<this <<" fd = "<<channel_ -> fd() << " state="<<state();
  assert(state_ == kDisconnected);
  assert(anyPtr == NULL);
}

// Connection析构前的最后一步，在handleClose的下一次poll后进行
// 关闭channel监听
void Connection::connDestroy()
{
  loop_ -> assertInLoopThread();
  // 若未通过handleClose进入关闭状态
  if(state_ == kConnected)
  {
    setState(kDisconnected);
    //然后进行关闭
    channel_ ->disableAll();
    //断开事件回调
    connCallback_(shared_from_this());
  }

  channel_ -> remove();
}

//开启Channel监听服务
void Connection::connEstablish()
{
  loop_ ->assertInLoopThread();
  LOG << state();
  assert(state_ == kConnecting);

  setState(kConnected);
  //加入监听列表 启动监听
  //Channel_->enableReading() => Channel::update() => EventLoop::ChannelUpdate() => Poller::ChannelUpdate()
  //channel_ -> tie(shared_from_this());
  channel_ -> enableRead();
  //连接事件回调
  connCallback_(shared_from_this());
}


/***************************回调函数，绑定给对应Channel**************************/

void Connection::handleRead()
{
  loop_ -> assertInLoopThread();
  int savedErrno = 0;
  ssize_t n = inputBuffer_.readFd(channel_ -> fd(), &savedErrno);
  // 若读取成功
  if(n > 0)
  {
    messCallback_(shared_from_this(), &inputBuffer_);
  }
  else if(n == 0)//未有数据但触发了handleRead,说明对方close连接，处理close事件，关闭连接
  {
    //LOG_INFO<<"handleRead => handleClose";
    handleClose();
  }
  else// 处理错误信息
  {
    errno = savedErrno;
    LOG_ERROR << "Connection::handleRead";
    handleError();
  }//END IF
}// END handleRead



// 有东西要写的时候才开启写监听
void Connection::handleWrite()
{
  loop_ -> assertInLoopThread();
  if(channel_ -> isWriting())
  {
    // 将inputBuffer_内容写入fd
    ssize_t n = write(channel_ -> fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
    // TODO 有bug则 if(channel_ -> isWriting())
    // 判断写入量
    if(n > 0)
    {
      //调整readerIndex
      outputBuffer_.retrieve(n);
      //若无东西可写，则关闭写操作，防止lt模式下写无限触发,并调用用户绑定的WriteCompleteCB
      if(outputBuffer_.readableBytes() == 0)
      {
        channel_ -> disableWrite(); 
        if(writeCompleteCallback_)  
          loop_ -> queueInLoop(std::bind(&Connection::writeCompleteCallback_, shared_from_this()));
        //在连接关闭的情况下，若有数据未写完，需写完再关（优雅关闭），
        //在等待时state_已经变成kDisconnecting,故要判断是否进入连接关闭状态
        if(state_ == kDisconnecting)
          shutdownInLoop();
      }
    }//END n > 0
    else//发送失败
    {
      LOG_ERROR << "Connection::handleWrite";
    }
  }//END if isWriting()
  else
    LOG_TRACE << "Connection fd = "<<channel_ -> fd() << " is down, no more writing";
}

// 被动关闭触发
void Connection::handleClose()
{
  loop_ -> assertInLoopThread();
  LOG_TRACE <<"fd = "<<channel_ -> fd() << " state="<<state();
  assert(state_ == kConnected || state_ == kDisconnecting);
  setState(kDisconnected);
  // 停止新的事件产生
  channel_ -> disableAll();
  // 创建一个Ptr增加引用计数，后通过closeCallback_传递给Server::removeConn(),
  // 再传递给Connection::connDestroy,将this生命周期延长至下次poll后
  // 防止从connection_移除this后便析构，造成handleEvent错误
  ConnectionPtr This(shared_from_this());
  connCallback_(This);
  //Server::removeConn从Server移除相关连接 => Server::removeConnInLoop => Connection::ConnDestroy(conn)从loop移除相关连接
  closeCallback_(This);
}

// TODO
void Connection::handleError()
{
  LOG_ERROR << "Connection::handleError ["<<name_<<"]";
}

//*************************************主动关闭
void Connection::shutdown()
{
  if(state_ == kConnected)
  {
    setState(kDisconnecting);
    loop_ -> runInLoop(std::bind(&Connection::shutdownInLoop, this));
  }
}

void Connection::shutdownInLoop()
{
  loop_ -> assertInLoopThread();
  // 要写完再关闭，优雅
  // 此时state_已经变成kDisconnecting, 写完后handleWrite会调用shutdownWrite
  if(!channel_ ->isWriting())
  {
    socket_ -> shutdownWrite();
  }
}

void Connection::forceClose()
{
  if(state_ == kConnected || state_ == kDisconnecting)
  {
    setState(kDisconnecting);
    loop_ -> runInLoop(std::bind(&Connection::forceCloseInLoop, this));
  }
}

void Connection::delayClose(double seconds)
{
  if(state_ == kConnected || kDisconnecting)
  {
    setState(kDisconnecting);
    loop_ -> runAfter(seconds, std::bind(&Connection::handleClose,this));
  }
}

void Connection::forceCloseInLoop()
{
  loop_ -> assertInLoopThread();
  if(state_ == kConnected || state_ == kDisconnecting)
  {
    handleClose();
  }
}

/*******************************************TODO 发送数据**************************************/

void Connection::send(const std::string& message)
{
  if(state_ == kConnected)
  {
    if(loop_ -> isInLoopThread())
    {
      sendInLoop(message);
    }
    else
    {
      loop_ -> runInLoop(std::bind(&Connection::sendInLoop, this, message));
    }
  }
}

/*
void Connection::send(const char* data, size_t len)
{
  if(state_ == kConnected)
  {
    if(loop_ -> isInLoopThread())
    {
      sendInLoop(data, len);
    }
    else
    {
      loop_ -> runInLoop(std::bind(&Connection::sendInLoop, this, data, len));
    }
  }
}
*/

void Connection::send(Buffer* buf)
{ 
  if(state_ == kConnected)
  {
    if(loop_ -> isInLoopThread())
    {
      sendInLoop(buf -> toString());
    }
    else
    {
      // 传递给const引用的临时变量会被延长生命周期
      loop_ -> runInLoop(std::bind(&Connection::sendInLoop, this, buf -> toString() ));
    }
  }
}

/*
 * 写入数据
 * 1.如果Channel没有监听可写事件且输出缓冲区为空，说明之前没有出现内核缓冲区满的情况，直接写进内核
 * 2.如果写入内核出错，且出错信息(errno)是EWOULDBLOCK，说明内核缓冲区满，将剩余部分添加到应用层输出缓冲区
 * 3.如果之前输出缓冲区为空，那么就没有监听内核缓冲区(fd)可写事件，开始监听
 */
void Connection::sendInLoop(const std::string& message)
{
  loop_ -> assertInLoopThread();
  //记录写入内核缓冲区的字节数
  ssize_t nwrote = 0;
  // 记录未写入缓冲区的字节数
  size_t remaining = message.size();
  if(state_ == kDisconnected)
  {
    LOG_WARN << "disconnected, give up writing";
    return;
  }
  // 如果Channel没有监听可写事件且输出缓冲区为空，说明之前没有出现内核缓冲区满有数据未完全写入的情况，直接写进内核
  // 如果输出缓冲区有数据，就不能尝试发送数据了，否则数据会乱
  if(!channel_ -> isWriting() && outputBuffer_.readableBytes())
  {
    nwrote = write(channel_ -> fd(), message.data(), message.size());
    //若有数据写入
    if(nwrote >= 0)
    {
      remaining = message.size() - nwrote;
      if(remaining == 0 && writeCompleteCallback_)
        loop_ -> queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));// 处理玩channel所有事件后在进行，因为可能也要调用channel
    }
    else // nwrote < 0, 有错
    {
      if(errno != EWOULDBLOCK)// EWOULDBLOCK表示缓存区已满，其他都有需解决错误
      {
        LOG_ERROR <<"Connection::sendInLoop";
        //TODO: EPIPE ECONNRESET? 出错也不能加入应用层缓存区
      }
    }
  }// END if !channel_ -> isWriting() ...

  assert(remaining <= message.size());
  // 若没有完全写入tcp缓存区，则写入应用层缓存区并开启channel的可写
  if(remaining > 0)
  {
    outputBuffer_.append(message.data() + nwrote, remaining);
    if(!channel_ -> isWriting())
      channel_ -> enableWrite();
  }
}

