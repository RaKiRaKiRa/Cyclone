/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-07-18 15:34
 * Last modified : 2019-08-28 12:09
 * Filename      : Buffer.h
 * Description   : 
 **********************************************************/

#ifndef BUFFER_H
#define BUFFER_H

#include "base/noncopyable.h"
#include "SocketOpts.h"
#include <vector>
#include <algorithm>
#include <string.h>  //memcpy
#include <memory>
#include <assert.h>

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

class Buffer : noncopyable
{
public:
  static const size_t kCheapPrepend = 8;    //预留空间，用于防止粘包
  static const size_t kInitialSize  = 1024; //默认数据存储大小
  static const size_t kHeaderLen    = 4;    //default Header Size

  explicit Buffer(size_t initialSize = kInitialSize):
    buffer_(kCheapPrepend + initialSize),
    readerIndex_(kCheapPrepend),
    writerIndex_(kCheapPrepend)
  {}

  //清空数据
  void clear()
  {
    readerIndex_ = kCheapPrepend;
    writerIndex_ = kCheapPrepend;
  }

  //获取buffer当前信息
  size_t readableBytes() const
  {
    return writerIndex_ - readerIndex_;
  }
  size_t writableBytes() const
  {
    return buffer_.size() - writerIndex_;
  }
  size_t prependableBytes() const
  {
    return readerIndex_;
  }

  //获得数据起始位置（若有包头则从包头开始）
  const char* peek() const
  {
    return begin() + readerIndex_;
  }

  //获得可写起始位置
  char* beginWrite()
  {
    return begin() + writerIndex_;
  }

  const char* beginWrite() const
  {
    return begin() + writerIndex_;
  }

  const char* findCRLF(const char* begin) const
  {
    assert(begin >= peek());
    assert(begin < beginWrite());
    const char* crlf = std::search(begin, beginWrite(), kCRLF, kCRLF + 2);
    return crlf == beginWrite() ? NULL : crlf;
  }

  const char* findEOF(const char* begin) const
  {
    assert(begin >= peek());
    assert(begin < beginWrite());
    const char* eof =static_cast<const char*>( memchr(begin, '\n', beginWrite() - begin) );
    return eof;
  }

  const char* findCRLF() const
  {
    return findCRLF(peek());
  }

  const char* findEOF() const 
  {
    return findEOF(peek());
  }
  /*************************************  读  **********************************/

  //拿出len字节的数据
  void retrieve(size_t len)
  {
    assert(len <= readableBytes());
    if(len < readableBytes())
    {
      readerIndex_ += len;
    }
    else
    {
      clear();
    }
  }

  //拿出len字节并转化为字符串返回
  std::string toString(size_t len)
  {
    assert(len <= readableBytes());
    std::string result(peek(), len);
    retrieve(len);
    return result;
  }

  std::string toString()
  {
    return toString(readableBytes());
  }
  
  //从fd读取并存入buffer_，返回字节数
  size_t readFd(int fd, int* savedErrno);

  /*************************************  写  **********************************/

  //移动writerIndex_
  void hasWrite(size_t len)
  {
    assert(len <= writableBytes());
    writerIndex_ += len;
  }

  //写入内容
  void append(const char* data, size_t len)
  {
    //写前首先确保要有足够容纳len字节数据的空间
    ensureWritable(len);
    //然后写入数据
    std::copy(data, data+len, beginWrite());
    //移动writerIndex_
    hasWrite(len);
  }

  void prepend(const void* data, size_t len)
  {
    assert(len <= prependableBytes());
    readerIndex_ -= len;
    const char* d = static_cast<const char*>(data);
    std::copy(d, d + len, begin() + readerIndex_);
  }

  void append(std::string& str)
  {
    append(str.data(), str.size());
  }


  void ensureWritable(size_t len)
  {
    //若空间不足则通过resize开辟新空间
    if(writableBytes() < len)
      makeSpace(len);
    assert(writableBytes() >= len);
  }

  //使可以再放入len字节
  /* 其策略为：
   * 在多次从缓冲区读数据后，readerIndex会后移很多，导致预留空间变大
   * 在增大空间之前，先判断调整预留空间的大小到8byte后能否容纳要求的数据
   * 如果可以，则将预留空间缩小为8字节（默认的预留空间大小）
   * 如果不可以，那么就只能增加空间
   */
  void makeSpace(size_t len)
  {
    //若buffer_前后端空余不可以容下len数据+包头，则resize
    if(writableBytes() + prependableBytes() < len + kCheapPrepend)
    {
      buffer_.resize(writerIndex_ + len);
    }
    else//通过调整可以容纳则调整
    {
      assert(readerIndex_ > kCheapPrepend);
      size_t readable = readableBytes();//数据数
      //数据前移，并调整指针
      std::copy(begin() + readerIndex_, begin() + writerIndex_, begin() + kCheapPrepend);
      readerIndex_ = kCheapPrepend;
      writerIndex_ = readerIndex_ + readable;
      assert(readable == readableBytes());
    }

  }

  /*************************************  包头操作  **********************************/
  //包头在储存时为二进制网络序，使用时需转化
  //获取包头并返回（移动readerIndex_)
  int32_t readHeader()
  {
    int32_t header = getHeader();
    retrieve(kHeaderLen);
    return header;
  }

  //获取包头并返回（未移动readerIndex_)
  int32_t getHeader() const
  {
    assert(readableBytes() >= kHeaderLen);
    int32_t header = 0;
    memcpy(&header, peek(), sizeof header);
    return networkToHost32(header);
  }

  //写入一个包头
  void setHeader(int32_t x)
  {
    assert(kHeaderLen <= prependableBytes());
    int32_t len = hostToNetwork32(x);
    prepend(&len, kHeaderLen);
  }

private:

  char* begin() 
  {
    return buffer_.data();
  }
  const char* begin() const
  {
    return buffer_.data();
  }

  std::vector<char> buffer_;                //数据是通过vector存放，可以使用vector封装好的resize，方便
  size_t readerIndex_;
  size_t writerIndex_;
  static const char kEOF;
  static const char kCRLF[];
};



#endif
