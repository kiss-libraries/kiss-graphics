#ifndef _KISS_GRAPHICS_PIPE_DISPLAY_HPP_
#define _KISS_GRAPHICS_PIPE_DISPLAY_HPP_

#include "kiss-graphics/display.hpp"

namespace kiss
{
  namespace graphics
  {
    class PipeDisplay : public Display
    {
    public:
      PipeDisplay(const unsigned short port);
      ~PipeDisplay();
      
      virtual bool open(const unsigned width, const unsigned height);
      virtual void update(const Context &context);
      virtual void close();
      
    private:
      unsigned short _port;
      unsigned _width;
      unsigned _height;
      void *_handle;
    };
  }
}

#endif