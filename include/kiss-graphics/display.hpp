#ifndef _KISS_GRAPHICS_DISPLAY_HPP_
#define _KISS_GRAPHICS_DISPLAY_HPP_

#include "context.hpp"

namespace kiss
{
  namespace graphics
  {
    class Display
    {
    public:
      virtual bool open(const unsigned width, const unsigned height) = 0;
      virtual void update(const Context &context) = 0;
      virtual void close() = 0;
    };
  }
}

#endif