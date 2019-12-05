/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-08 21:41
 * Last modified : 2019-11-16 02:02
 * Filename      : Client.cc
 * Description   : 
 **********************************************************/

#include "Client.h"
#include "base/Logging.h"
#include "EventLoop.h"
#include "SocketOpts.h"
#include "Callback.h"
#include <functional>
#include <stdio.h>

Client::Client(EventLoop* loop, sockaddr_in& serverAddr, const std::string& name):
    loop_(loop),
    connector_(new Connector(loop, serverAddr)),
    connCallback_(defaultConnCallback),
    messCallback_(defaultMessCallback),
    name_(name),
    nextConnId_(1),
    retry_(false),
    connect_(false)
{
    connector_->setNewConnCallback(std::bind(&Client::newConnection, this, _1)); // void newConnection(int sockfd);
    
    LOG_INFO << "Client::Client [" << name_ << "] - connector " << connector_.get();
}

Client::Client(EventLoop* loop, std::string ip, uint16_t port, const std::string& name):
    loop_(loop),
    connCallback_(defaultConnCallback),
    messCallback_(defaultMessCallback),
    name_(name),
    nextConnId_(1),
    retry_(false),
    connect_(false)
{
    sockaddr_in serverAddr;
    fromIpPort(ip.c_str(), port);
    connector_.reset(new Connector(loop_, serverAddr));
    connector_->setNewConnCallback(std::bind(&Client::newConnection, this, _1)); // void newConnection(int sockfd);
    
    LOG_INFO << "Client::Client [" << name_ << "] - connector " << connector_.get();
}


Client::Client(EventLoop* loop, uint16_t port, const std::string& name):
    loop_(loop),
    connCallback_(defaultConnCallback),
    messCallback_(defaultMessCallback),
    name_(name),
    nextConnId_(1),
    retry_(false),
    connect_(false)
{
    sockaddr_in serverAddr;
    fromPort(port);
    connector_.reset(new Connector(loop_, serverAddr));
    connector_->setNewConnCallback(std::bind(&Client::newConnection, this, _1)); // void newConnection(int sockfd);
    LOG_INFO << "Client::Client [" << name_ << "] - connector " << connector_.get();
}

// 主要是处理对Connection_的断开操作，
// 需要判断是否有连接
// TODO
Client::~Client()
{
    LOG_INFO << "Client::~Client [" << name_ << "] - connector " << connector_.get();
    ConnectionPtr conn;
    {
        MutexGuard lock(mutex_);
        conn = connection_;
    }

    if(conn)
    {
        assert(loop_ == conn->loop());
        // connection断开后不应再尝试重试
        CloseCallback cb = std::bind(&Client::removeConnectionAfterDestruct, loop_, _1);
        // 将关闭操作放置与下一次loop
        loop_->runInLoop(std::bind(&Connection::setCloseCallback, conn, cb));
        conn->forceClose();
    }
    else
    {
        connector_->stop();
    }
    
}

void Client::connect()
{
    connect_ = true;
    connector_->start();
}

void Client::disconnect()
{

    //停止连接后依然可能由连接存在
    connect_ = false;
    {
        MutexGuard lock(mutex_);
        if(connection_)
        {
            connection_->shutdown();
        }
    }
}

void Client::stop()
{
    connect_ = false;
    connector_->stop(); 
}

void Client::newConnection(int sockfd)
{
    loop_->assertInLoopThread();
    char buf[64];
    sockaddr_in peerAddr = getPeerAddr(sockfd);
    sockaddr_in localAddr = getLocalAddr(sockfd);
    snprintf(buf, sizeof buf, ":%s#%d", toIpPort(peerAddr).c_str(), nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;
    ConnectionPtr conn(new Connection(loop_, connName, localAddr, peerAddr, sockfd));
    conn->setConnCallback(connCallback_);
    conn->setMessCallback(messCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(std::bind(&Client::removeConnection, this, _1));

    {
        MutexGuard lock(mutex_);
        connection_ = conn;
    }
    conn->connEstablish();
}

// 作为CloseCallback，当发现关闭时connect_为true，启动重连
void Client::removeConnection(const ConnectionPtr& conn)
{
    loop_->assertInLoopThread();
    assert(loop_ == conn->loop());
    // 延长生命周期到connDestroy，因为此刻正在handleEvent,计数+1
    loop_->queueInLoop(std::bind(&Connection::connDestroy, conn));

    {
        MutexGuard lock(mutex_);
        assert(conn == connection_);
        // 计数-1，仅靠connDestroy的参数，延续了本函数的参数
        connection_.reset();
    }
    // STAR!!!
    // 尝试重连，在一定时间后调用Connector::startInLoop
    if(retry_ && connect_)
    {
        LOG_TRACE << "connect [" << name_ << "] Reconnect to " << toIpPort(connector_ -> serverAddr());
        connector_->restart();
    }
}


// 析构时将connection_的closecallback设为这个
// 去掉了重连，重置Connection等操作
// 在Client析构后仍可以调用，且通过function延长生命周期到connDestroy
void Client::removeConnectionAfterDestruct(EventLoop* loop, const ConnectionPtr& conn)
{
    loop->queueInLoop(std::bind(&Connection::connDestroy, conn));
}
