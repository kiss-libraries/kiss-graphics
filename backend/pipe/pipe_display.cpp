#include "pipe_display.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

using namespace kiss::graphics;

namespace
{
  struct PipeHandle
  {
    int fd;
  };
  
  
  struct __attribute__((packed)) DisplayHeader
  {
    int pid;
    unsigned width;
    unsigned height;
  };
}

PipeDisplay::PipeDisplay(const unsigned short port)
  : _port(port)
  , _handle(0)
{
  
}

PipeDisplay::~PipeDisplay()
{
  close();
}

bool PipeDisplay::open(const unsigned width, const unsigned height)
{
  const int fd = ::open("/tmp/display", O_RDWR | O_NONBLOCK);
  if(fd < 0) return false;
  
  _width = width;
  _height = height;
  
  PipeHandle *p = new PipeHandle;
  p->fd = fd;
  
  _handle = reinterpret_cast<void *>(p);
  
  return true;
}

void PipeDisplay::update(const Context &context)
{
  if(!_handle) return;
  PipeHandle *const p = reinterpret_cast<PipeHandle *>(_handle);
  
  DisplayHeader header;
  header.pid    = getpid();
  header.width  = _width;
  header.height = _height;
  
  write(p->fd, &header, sizeof(header));
  write(p->fd, context.backing(), context.size() * sizeof(Context::Rgb));
}

void PipeDisplay::close()
{
  if(!_handle) return;
  PipeHandle *const p = reinterpret_cast<PipeHandle *>(_handle);
  ::close(p->fd);
  delete p;
  _handle = 0;
}