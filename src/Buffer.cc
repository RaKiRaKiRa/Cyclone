/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-20 15:51
 * Last modified : 2019-07-30 20:55
 * Filename      : Buffer.cc
 * Description   : 
 **********************************************************/

#include "Buffer.h"
const char Buffer::kEOF = '\n';
const char Buffer::kCRLF[] = "\r\n";
/*
 * 从tcp缓冲区(sockfd)中读取数据，存放到应用层缓冲区中
 *   两种情况
 *      1.应用层缓冲区足以容纳所有数据
 *        直接读取到buffer_中,即vec[0]
 *      2.应用层缓冲区不够
 *        开辟一段栈空间（128k）大小，即vec[1],使用分散读（readv）系统调用读取数据
 *        然后为buffer_开辟更大的空间，存放读到栈区的那部分数据
 *
 *   为什么不在Buffer构造时就开辟足够大的缓冲区
 *      1.每个tcp连接都有输入/输出缓冲区，如果连接过多则内存消耗会很大
 *      2.防止客户端与服务器端数据交互比较少，造成缓冲区的浪费
 *      3.当缓冲区大小不足时，利用vector内存增长的优势，扩充缓冲区
 *
 *   为什么不在读数据之前判断一下应用层缓冲区是否可以容纳内核缓冲区的全部数据
 *      1.采用这种方式就会调用一次recv，传入MSG_PEEK,即recv(sockfd,, extrabuf, sizeof(extrabuf), MSG_PEEK)
 *        可根据返回值判断缓冲区还有多少数据没有接收，然后再调用一次recv从内核冲读取数据
 *      2.但是这样会执行两次系统调用，得不偿失，尽量使用一次系统调用就将所有数据读出，这就需要一个很大的空间
 *
 *   struct iovec
 *   I/O vector，与readv和wirtev操作相关的结构体。readv和writev函数用于在一次函数调用中读、写多个非连续缓冲区。有时也将这两个函数称为散布读（scatter read）和聚集写（gather write）。
 *   readv和writev可以在一个原子操作中读取或写入多个缓冲区。
 *      1.iov_base，存放数据的缓冲区起始位置，写时往这个位置写入iov_len个字节，读时从这个位置读出iov_len个字节
 *      2.iov_len，要读入多少数据从内核缓冲区/要写入多少数据到内核缓冲区
 *
 *   readv(int fd, const struct iovec *iov, int iovcnt);分散读
 *   writev(int fd, const struct iovec *iov, int iovcnt);集中写
 */
size_t Buffer::readFd(int fd, int* savedErrno)
{
  //另开辟64kB的空间作为读取缓冲区预留空间，保证一次readv读取完，减少系统调用次数
  //足以几首千兆网在500us全速传输的数据
  char extrabuf[65535]; 
  struct iovec vec[2];
  const size_t writable = writableBytes();

  vec[0].iov_base = begin() + writerIndex_;
  vec[0].iov_len  = writable;
  vec[1].iov_base = extrabuf;
  vec[1].iov_len  = sizeof extrabuf;

  //分散读
  const ssize_t n = readv(fd, vec, 2);

  if(n < 0)
  {
    *savedErrno = errno;
  }
  // 若缓冲区剩余空间足够用，直接移动writerIndex_
  else if(static_cast<size_t>(n) <= writable)
  {
    writerIndex_ += n;
  }
  // 若缓冲区空间不够用，将extrabuf读取内容加入
  else
  {
    writerIndex_ = buffer_.size();
    append(extrabuf, n - writable);
  }
  return n;
}
