/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-30 17:33
 * Last modified : 2019-07-16 22:36
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

typedef std::function<void (const ConnectionPtr&)> ConnCallback;//连接事件

#endif
