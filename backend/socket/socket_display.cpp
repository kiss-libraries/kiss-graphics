#include "socket_display.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <chrono>
#include <zlib.h>
#include <assert.h>

#include <deque>
#include <vector>
#include <iostream>

using namespace kiss::graphics;

#define ZLIB_CHUNK 32768U

namespace
{
  struct SocketHandle
  {
    int fd;
    std::chrono::high_resolution_clock::time_point last;
  };
  
  
  struct __attribute__((packed)) DisplayHeader
  {
    int messageSize;
    int pid;
    unsigned width;
    unsigned height;
    unsigned payloadSize;
  };
  
  bool setSocketBlocking(const int fd, const bool blocking)
  {
     if (fd < 0) return false;

  #ifdef WIN32
     unsigned long mode = blocking ? 0 : 1;
     return ioctlsocket(fd, FIONBIO, &mode) == 0 ? true : false;
  #else
     int flags = fcntl(fd, F_GETFL, 0);
     if(flags < 0) return false;
     flags = blocking ? flags & ~O_NONBLOCK : flags | O_NONBLOCK;
     return fcntl(fd, F_SETFL, flags) == 0 ? true : false;
  #endif
  }
  
  int deflate(const unsigned char *data, unsigned size, std::vector<unsigned char> &outData, int level)
  {
    int ret, flush;
    unsigned have;
    z_stream strm;
    unsigned char in[ZLIB_CHUNK];
    unsigned char out[ZLIB_CHUNK];

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, level);
    if (ret != Z_OK) return ret;

    do {
      strm.avail_in = std::min(size, ZLIB_CHUNK);
      size -= strm.avail_in;
      memcpy(in, data, strm.avail_in);
      flush = !size ? Z_FINISH : Z_NO_FLUSH;
      strm.next_in = in;

      do {
        strm.avail_out = ZLIB_CHUNK;
        strm.next_out = out;
        ret = deflate(&strm, flush);
        assert(ret != Z_STREAM_ERROR);
        have = ZLIB_CHUNK - strm.avail_out;
        outData.insert(outData.end(), out, out + have);
      } while (strm.avail_out == 0);
      assert(strm.avail_in == 0);
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);

    deflateEnd(&strm);
    return Z_OK;
  }
}

SocketDisplay::SocketDisplay(const std::string &address, const unsigned short port)
  : _address(address)
  , _port(port)
  , _handle(0)
{
  
}

SocketDisplay::~SocketDisplay()
{
  close();
}

bool SocketDisplay::open(const unsigned width, const unsigned height)
{
  const int fd = socket(AF_INET, SOCK_STREAM, 0);
  if(fd < 0) return false;
  
  int yes = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));
  
  sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(_address.c_str());
  addr.sin_port = htons(_port);
  
  if(connect(fd, (sockaddr *)&addr, sizeof(addr)) < 0)
  {
    ::close(fd);
    return false;
  }
  
  if(!setSocketBlocking(fd, false))
  {
    ::close(fd);
    return false;
  }
  
  _width = width;
  _height = height;
  
  SocketHandle *p = new SocketHandle;
  p->fd = fd;
  
  _handle = reinterpret_cast<void *>(p);
  
  return true;
}

void SocketDisplay::update(const Context &context)
{
  if(!_handle) return;
  SocketHandle *const p = reinterpret_cast<SocketHandle *>(_handle);
  
  auto t = std::chrono::high_resolution_clock::now();
  auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(t - p->last).count();
  if(millis < 50) return;
  
  DisplayHeader header;
  
  header.pid    = getpid();
  header.width  = _width;
  header.height = _height;
  
  const Context::Rgb *const backing = context.backing();
  
  //std::vector<unsigned char> payload;
  //deflate(reinterpret_cast<const unsigned char *>(backing), context.size() * sizeof(Context::Rgb), payload, 9);
  
  header.payloadSize = context.size() * sizeof(Context::Rgb);
  //header.payloadSize = payload.size();
  header.messageSize = sizeof(header) + header.payloadSize;
  
  send(p->fd, &header, sizeof(header), 0);
  send(p->fd, backing, context.size() * sizeof(Context::Rgb), 0);
  //send(p->fd, &payload[0], payload.size(), 0);
  
  p->last = t;
}

void SocketDisplay::close()
{
  if(!_handle) return;
  SocketHandle *const p = reinterpret_cast<SocketHandle *>(_handle);
  ::close(p->fd);
  delete p;
  _handle = 0;
}