/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-08-19 11:38
 * Last modified : 2019-09-01 20:23
 * Filename      : serverWithHeartBeat.cc
 * Description   : 
 **********************************************************/

#include "serverWithHeartBeat.h"
#include "EventLoop.h"

serverWithHeartBeat::serverWithHeartBeat(EventLoop* loop,const sockaddr_in& listenAddr, const std::string& name, bool ReusePort, int idleSec):
  server_(loop, listenAddr, name, ReusePort),
  bucketList_(idleSec),
  printStatus(false),
  idleSec_(idleSec)
{
  server_.setConnCallback(std::bind(&serverWithHeartBeat::onConnection, this, _1));
  server_.setMessCallback(std::bind(&serverWithHeartBeat::onMessage, this, _1, _2));
  server_.setWriteCompleteCallback(writeCompleteCallback_);

  loop -> runEvery(1.0, std::bind(&serverWithHeartBeat::onTimer, this));
  bucketList_.resize(idleSec);
  dumpConnectionBuckets();
}

void serverWithHeartBeat::setThreadNum(int numThread)
{
  server_.setThreadNum(numThread);
}

void serverWithHeartBeat::start()
{
  server_.start();
}

void serverWithHeartBeat::onConnection(const ConnectionPtr& conn)
{
  connCallback_(conn);
  
  // 新建立连接，建立新的Entry
  if(conn -> connected())
  {
    EntryPtr entry(new Entry(conn));
    bucketList_.back().insert(entry);
    dumpConnectionBuckets();
    // 存于对应Entry
    EntryWeakPtr entryWP(entry);
    entry -> setWP(entryWP);
    conn -> setAnyPtr(static_cast<void*>(entry.get()));
  }
  // 关闭连接，在connDestroy里调用
  else
  {
    assert(conn -> getAnyPtr() != NULL);
    conn -> setAnyPtr(NULL);
  }
}


void serverWithHeartBeat::onMessage(const ConnectionPtr& conn, Buffer* buffer_)
{ 
  messCallback_(conn, buffer_);

  // 每次有消息，则重新加入时间轮
  assert(conn -> getAnyPtr() != NULL);
  EntryWeakPtr entryWP = static_cast<Entry*>(conn -> getAnyPtr())->getWP();
  EntryPtr entry(entryWP.lock());
  if(entry)
  {
    bucketList_.back().insert(entry);
    dumpConnectionBuckets();
  }
  
}

void serverWithHeartBeat::onTimer()
{
  bucketList_.pop_front();
  bucketList_.push_back(Bucket());
  dumpConnectionBuckets();
}

void serverWithHeartBeat::dumpConnectionBuckets() const
{
  if(!printStatus)
    return ;
  LOG_INFO << "size = " << bucketList_.size();
  int idx = 0;
  for (BucketList::const_iterator bucketI = bucketList_.begin();
      bucketI != bucketList_.end();
      ++bucketI, ++idx)
  {
    const Bucket& bucket = *bucketI;
    printf("[%d] len = %zd : ", idx, bucket.size());
    for (const auto& it : bucket)
    {
      bool connectionDead = it->weakConn_.expired();
      printf("%p(%ld)%s, ", it.get(), it.use_count(),
          connectionDead ? " DEAD" : "");
    }
    puts("");
  }
}
