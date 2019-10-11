/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-08 21:21
 * Last modified : 2019-10-10 15:45
 * Filename      : Client.h
 * Description   : 
 **********************************************************/

#ifndef CYCLONE_CLIENT_H
#define CYCLONE_CLIENT_H
#include "Connector.h"
#include "Connection.h"
#include "base/MutexLock.h"

typedef std::shared_ptr<Connector> ConnectorPtr;

class Client : noncopyable
{
public:
    Client(EventLoop* loop, sockaddr_in& serverAddr, const std::string& name = "BaseClient");
    Client(EventLoop* loop, std::string ip, uint16_t port, const std::string& name = "BaseClient");
    Client(EventLoop* loop, uint16_t port, const std::string& name = "BaseClient");

    ~Client();

    // 开始使用Connector连接
    void connect();
    // 断开Connection连接
    void disconnect();
    // Connector停止发起连接操作
    void stop();

    // 设置与获取
    void setConnCallback(ConnCallback cb)
    {
        connCallback_ = std::move(cb);
    }
    void setMessCallback(MessCallback cb)
    {
        messCallback_ = std::move(cb);
    }
    void setWriteCompleteCallback(WriteCompleteCallback cb)
    {
        writeCompleteCallback_ = std::move(cb);
    }

    EventLoop* loop() const
    {
        return loop_;
    }

    bool retry() const
    {
        return retry_;
    }

    void setRetry(bool on)
    {
        retry_ = on;
    }

    std::string name() const
    {
        return name_;
    }

private:
    void newConnection(int sockfd);
    void removeConnection(const ConnectionPtr& conn);
    
    // 析构时将connection_的closecallback设为这个
    // 在Client析构后仍可以调用，且通过function延长生命周期到connDestroy
    static void removeConnectionAfterDestruct(EventLoop* loop, const ConnectionPtr& conn);

    EventLoop *loop_;
    ConnectorPtr connector_;
    ConnectionPtr connection_;

    ConnCallback connCallback_;
    MessCallback messCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    
    const std::string name_;
    int nextConnId_;
    bool retry_;
    bool connect_;

    MutexLock mutex_;
};

#endif
