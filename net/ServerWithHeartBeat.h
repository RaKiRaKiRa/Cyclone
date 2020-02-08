/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-18 23:11
 * Last modified : 2020-02-08 15:47
 * Filename      : serverWithHeartBeat.h
 * Description   : 
 **********************************************************/
#ifndef SERVERWITHHEARTBEAT_H
#define SERVERWITHHEARTBEAT_H

#include "base/noncopyable.h"
#include "Callback.h"
#include "Server.h"
#include <list>
#include <unordered_set>
#include <unordered_map>
#include "base/Logging.h"

class Server;
class EventLoop;

class serverWithHeartBeat : noncopyable
{
public:
  serverWithHeartBeat(EventLoop* loop, const sockaddr_in& listenAddr, const std::string& name = ""
      ,bool ReusePort = true, int idleSec = 20);

  void setThreadNum(int num);

  void start();
  
  //设置回调
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

  std::string name() const
  {
    return server_.name();
  }

  std::string ipPort() const
  {
    return server_.ipPort();
  }

  EventLoop* loop() const
  {
    return server_.loop();
  }

  int keepAliveTime() const
  {
    return idleSec_;
  }
  void setprintStatus(bool b)
  {
    printStatus = b;
  }

private:
  class Entry
  {
  public:
    explicit Entry(const ConnectionWeakPtr& weakConn) : weakConn_(weakConn){}
    //Entry在时间轮WeakConnectionList中由shared_ptr管理
    //每次连接都加入时间轮，一定时间内没有连接则析构Entry
    //完成shutdown
    ~Entry()
    {
      LOG_DEBUG << "~Entry()";
      ConnectionPtr conn = weakConn_.lock();
      // 若连接已关闭，Connection则已经析构，则lock()后为NULL
      if(conn)
        conn -> forceClose();
      LOG_DEBUG << "~Entry() end";

    }
    void setWP(std::weak_ptr<Entry> wp) 
    {
      wp_ = wp;
    }
    std::weak_ptr<Entry> getWP()
    {
      return wp_;
    }

    ConnectionWeakPtr weakConn_;
    std::weak_ptr<Entry> wp_;
  };
  typedef std::shared_ptr<Entry> EntryPtr;
  typedef std::weak_ptr<Entry>   EntryWeakPtr;
  typedef std::unordered_set<EntryPtr> Bucket;
  typedef std::list<Bucket> BucketList;
   
  void onConnection(const ConnectionPtr& conn);
  void onMessage(const ConnectionPtr& conn, Buffer* buffer_);
  void onWriteComplete(const ConnectionPtr& conn);
  void onTimer();
  void addEntry(const EntryPtr& entry);
  void dumpConnectionBuckets() const;

  ConnCallback          connCallback_;
  MessCallback          messCallback_;
  WriteCompleteCallback writeCompleteCallback_;
  Server server_;
  EventLoop *loop_;
  BucketList bucketList_;

  bool printStatus;
  int idleSec_;

};
#endif
