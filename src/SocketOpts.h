/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-09 23:54
 * Last modified : 2019-07-17 21:51
 * Filename      : SocketOpts.h
 * Description   : 
 **********************************************************/

#ifndef SOCKETOPTS_H
#define SOCKETOPTS_H
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <endian.h>
#include <string>

//网路字节序与主机字节序相互转换，be指big-endian，网络序是大端的

inline uint16_t hostToNetwork16(uint16_t host)
{
  return htobe16(host);
}

inline uint16_t networkToHost16(uint16_t network)
{
  return be16toh(network);
}

inline uint32_t hostToNetwork32(uint32_t host)
{
  return htobe32(host);
}

inline uint32_t networkToHost32(uint32_t network)
{
  return be32toh(network);
}

inline uint64_t hostToNetwork64(uint64_t host)
{
  return htobe64(host);
}

inline uint64_t networkToHost64(uint64_t network)
{
  return be64toh(network);
}


/*
 * struct in_addr{ in_addr_t s_addr; };
 * struct sockaddr_in{
 *  uint8_t        sin_len;
 *  sa_family_t    sin_family; //AF_INET
 *  in_port_t      sin_port;   //16bit TCP或UDP端口号
 *  struct in_addr sin_addr;   //32bit IPv4地址
 *  char           sin_zero[8];
 * };
 *
 * struct sockaddr{
 *  uint8_t     sa_len;
 *  as_family_t sa_family;
 *  char        sa_data[14];
 * };
 */



typedef sockaddr SA;

inline const SA* sockaddr_cast(const sockaddr_in *addr)
{
  return static_cast<const SA*>(static_cast<const void*>(addr));
}

inline SA* sockaddr_cast(sockaddr_in *addr)
{
  return static_cast<SA*>(static_cast<void*>(addr));
}



//×××××××××××××××对socket通信api进行封装，增加报错×××××××××××××

//创建一个非阻塞socketFd
int createNonblockingSockfd();

int Connect(int sockfd, const struct sockaddr_in* addr);

void Bind(int sockfd, const struct sockaddr_in* addr);

void Listen(int sockfd);

int Accept(int sockfd, sockaddr_in* addr);

//仅关闭WR,用于优雅关闭,不用close保证接收数据的完整性
//TCP 是全双工，shutdownWrite() 关闭了“写”方向的连接，保留了“读”方向，这称为half-close。
//如果直接 close(socket_fd)，那么 socket_fd 就不能读或写了。
//用 shutdown的效果是，如果对方已经发送了数据，这些数据还“在路上”，那么不会漏收这些数据。
//这种关闭连接的方式对对方也有要求，
//对方read() 到 0 字节之后会主动关闭连接（无论 shutdownWrite() 还是 close()）
//完整的流程是：我们发完了数据，于是 shutdownWrite，发送 TCP FIN 分节，对方会读到 0 字节，
//然后对方通常会关闭连接，这样 muduo 会读到 0 字节，然后 muduo 关闭连接。
void ShutdownWrite(int sockfd);

void Close(int sockfd);

//×××××××××××××××对socket设置和部分api进行封装××××××××××××××××××××

//设置非阻塞
void setNonblock(int sockfd, bool on);

void setReusePort(int sockfd, bool on);  

void setReuseAddr(int sockfd, bool on);  

//开关Nagle
void setNodelay(int sockfd, bool on);

//开关Tcp默认心跳
void setKeepAlive(int sockfd, bool on);

//将addr中的ip:port 输出给buf
void toIpPort(char* buf, size_t size, const struct sockaddr_in *addr);
std::string toIpPort(const struct sockaddr_in *addr);

//将ip port 传给addr
void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in *addr);
void fromPort(uint16_t port, struct sockaddr_in *addr);

//int getSocketError(int sockfd);


struct sockaddr_in getAddr(int sockfd);  

#endif
