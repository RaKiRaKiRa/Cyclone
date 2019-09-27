/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-09-27 21:13
 * Last modified : 2019-09-27 22:51
 * Filename      : Service.h
 * Description   : 
 **********************************************************/
#ifndef SERVICE_H
#define SERVICE_H

#include "../src/base/noncopyable.h"
#include "../src/Connection.h"
#include "json.hpp"
#include <unordered_map>

typedef nlohmann::json Json;

class Service: public noncopyable
{
public:
  typedef std::function<void(ConnectionPtr&, Json&)> handle;
  Service();
  ~Service();

  // 功能handle调用
  void login(ConnectionPtr& conn, Json&);

  void loginOut(ConnectionPtr& conn, Json&);

  void region(ConnectionPtr& conn, Json&);

  void addFriend(ConnectionPtr& conn, Json&);

  void addGroup(ConnectionPtr& conn, Json&);

  void createGroup(ConnectionPtr& conn, Json&);

  void offlineMsg(ConnectionPtr& conn, Json&);

  void oneChat(ConnectionPtr& conn, Json&);

  void groupChat(ConnectionPtr& conn, Json&);

  void Handle(int32_t &msgid, ConnectionPtr& conn, Json& json)
  {
    handleMap_[msgid](conn, json);
  }


private:
  std::unordered_map<int32_t, handle> handleMap_;
  

};


#endif
