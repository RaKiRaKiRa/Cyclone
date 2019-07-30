/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-30 17:33
 * Last modified : 2019-07-28 19:54
 * Filename      : Callback.h
 * Description   : 
 **********************************************************/

#ifndef CALLBACK_H
#define CALLBACK_H

#include <functional>
#include <memory>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

typedef std::function<void()> TimerCallback;


class Buffer;
class Connection;
typedef std::shared_ptr<Connection> ConnectionPtr;

typedef std::function<void (const ConnectionPtr&)> ConnCallback;          //连接事件
typedef std::function<void (const ConnectionPtr&, Buffer*)> MessCallback; //消息到达事件，fd可读
typedef std::function<void (const ConnectionPtr&)> CloseCallback;
typedef std::function<void (const ConnectionPtr&)> WriteCompleteCallback; // 数据已从应用层写入缓冲区，并不代表对方已收到

void defaultConnCallback(const ConnectionPtr& ptr);
void defaultMessCallback(const ConnectionPtr& conn, Buffer* buf);
#endif
