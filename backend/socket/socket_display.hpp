#ifndef _KISS_GRAPHICS_SOCKET_DISPLAY_HPP_
#define _KISS_GRAPHICS_SOCKET_DISPLAY_HPP_

#include "kiss-graphics/display.hpp"
#include <string>

namespace kiss
{
  namespace graphics
  {
    class SocketDisplay : public Display
    {
    public:
      SocketDisplay(const std::string &address, const unsigned short port);
      ~SocketDisplay();
      
      virtual bool open(const unsigned width, const unsigned height);
      virtual void update(const Context &context);
      virtual void close();
      
    private:
      std::string _address;
      unsigned short _port;
      
      unsigned _width;
      unsigned _height;
      void *_handle;
    };
  }
}

#endif