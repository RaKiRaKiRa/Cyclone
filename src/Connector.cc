/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-07 00:44
 * Last modified : 2019-10-07 20:32
 * Filename      : Connector.cc
 * Description   : 
 **********************************************************/

#include "Connector.h"
#include "Channel.h"
#include "EventLoop.h"
#include "base/Logging.h"
#include "errno.h"

Connector::Connector(EventLoop* loop, sockaddr_in& addr):
  loop_(loop),
  serverAddr_(addr),
  started_(false),
  state_(kDisconnected),
  retryDelayMs_(kInitRetryDelayMs)
{
  LOG_DEBUG << "connector [" << this <<"]";
}

Connector::~Connector()
{
  if(!id_.isNull)
  {
    loop_->cancel(id_);
  }
    
  LOG_DEBUG << "~connector [" << this <<"]";
}

void Connector::start()
{
  assert(!started_);
  started_ = true;
  loop_->runInLoop(std::bind(&Connector::startInLoop, this));
}

// 会被start()和retry()调用，
// connect失败都调用retry()，retry会过一会儿再重新startInLoop
// 这段时间内可能会stop，故判断是否开启连接
void Connector::startInLoop()
{
  assert(state_ == kDisconnected);
  if(!id_.isNull())
    id_.setNull();
  // 判断是否启动，用于retry放入定时器后，调用重连时判断
  if(started_)
  {
    connect();
  }
  else
  {
    LOG_DEBUG << "is not connect";
  }
}

void Connector::connect()
{
  int sockfd = createNonblockingSockfd();
  int ret = Connect(sockfd, &serverAddr_);
  int savedErrno = (ret == 0) ? 0 : errno;
  switch (savedErrno)
  {
    // 等待连接建立的情况
    case 0:
    case EINPROGRESS:     //套接字为非阻塞套接字，且连接请求没有立即完成
    case EINTR:           //系统调用的执行由于捕获中断而中止
    case EISCONN:         //已经连接到该套接字
      connecting(sockfd);
      break;

    //稍后重试的情况
    case EAGAIN:          //没有足够空闲的本地端口
    case EADDRINUSE:      //本地地址处于使用状态
    case EADDRNOTAVAIL:   //
    case ECONNREFUSED:    //远程地址并没有处于监听状态
    case ENETUNREACH:     //网络不可到达
      retry(sockfd);
      break;

    case EACCES:           //用户试图在套接字广播标志没有设置的情况下连接广播地址或由于防火墙策略导致连接失败。
    case EPERM:            //用户试图在套接字广播标志没有设置的情况下连接广播地址或由于防火墙策略导致连接失败。
    case EAFNOSUPPORT:     //参数serv_add中的地址非合法地址
    case EALREADY:         //套接字为非阻塞套接字，并且原来的连接请求还未完成
    case EBADF:            //非法的文件描述符
    case EFAULT:           //指向套接字结构体的地址非法
    case ENOTSOCK:         //文件描述符不与套接字相关
      LOG_ERROR << "connect error in Connector::startInLoop " << savedErrno;
      Close(sockfd);
      break;

    default:
      LOG_ERROR << "Unexpected error in Connector::startInLoop " << savedErrno;
      Close(sockfd);
      break;
  }
}

// 根据建立的连接生成Channel 判断连接是否可写,再handleWrite中继续下一步
void Connector::connecting(int sockfd)
{
  setState(kConnecting);
  assert(!channel_);
  channel_.reset(new Channel(loop_, sockfd));
  channel_->setWriteCallback(std::bind(&Connector::handleWrite, this));
  channel_->setErrorCallback(std::bind(&Connector::handleError, this));
  channel_->enableWrite();
}

// 可读表示连接已建立
void Connector::handleWrite()
{
  LOG_TRACE << "Connector::handleWrite " << state_;
  if(state_ == kConnecting)
  {
    int sockfd = removeAndResetChannel();
    int err = getSocketError(sockfd);
    //无错返回0
    if(err)
    {
      LOG_WARN << "Connector::handleWrite - ERROR = "<< err << " " << strerror(err);
      retry(sockfd);
    }
    else
    {
      setState(kConnected);
      if(started_)
      {
        newConnCallback_(sockfd);
      }
      else
      {
        Close(sockfd);
      }
    }
  }
  else
  {
    LOG_DEBUG << "Connector::handleWrite is not kConnecting, it is " << state_;
  }
}


void Connector::stop()
{
  started_ = false;
  loop_->runInLoop(std::bind(&Connector::stopInLoop, this));
}

void Connector::stopInLoop()
{
  // 将正在连接改为断开连接
  // 若已连接，则无视
  // 断开连接操作由Client和Connection进行
  if(state_ == kConnecting)
  {
    setState(kConnected);
    int sockfd = removeAndResetChannel();
    // 防止断开期间又开启
    retry(sockfd);
  }
}

void Connector::retry(int sockfd)
{
  Close(sockfd);
  setState(kDisconnected);
  if(started_)
  {
    LOG_INFO << "Connector::retry - Retry connecting to " << toIpPort(serverAddr_) << "in " << retryDelayMs_ << " ms";
    id_ = loop_->runAfter(retryDelayMs_ / 1000.0, std::bind(&Connector::startInLoop, this));
    retryDelayMs_ *= 2;
    if(retryDelayMs_ > kMaxRetryDelayMs)
      retryDelayMs_ = kMaxRetryDelayMs;  
  }
  else
  {
    LOG_DEBUG << "do not connect";
  }
}

int Connector::removeAndResetChannel()
{
  channel_->disableAll();
  channel_->remove();
  int sockfd = channel_->fd();
  // 本函数会在handleWrite里调用，也就是说我们正处于Channel::handleEvent
  // 故不能在这个reset
  loop_->queueInLoop(std::bind(&Connector::resetChannel, this));
  return sockfd;
}

void Connector::resetChannel()
{
  channel_.reset();
}

// 报错并retry
void Connector::handleError()
{
  LOG_ERROR << "Connector::handleError state = " << state_;
  if(state_ == kConnecting)
  {
    int sockfd = removeAndResetChannel();
    int err = getSocketError(sockfd);
    LOG_TRACE << "SO_ERROR = " << strerror(err);
    retry(sockfd);
  }
}
