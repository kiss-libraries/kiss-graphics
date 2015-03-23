#ifndef _KISS_GRAPHICS_SDL_DISPLAY_HPP_
#define _KISS_GRAPHICS_SDL_DISPLAY_HPP_

#include "kiss-graphics/display.hpp"

namespace kiss
{
  namespace graphics
  {
    class SdlDisplay : public Display
    {
    public:
      SdlDisplay();
      ~SdlDisplay();
      
      virtual bool open(const unsigned width, const unsigned height);
      virtual void update(const Context &context);
      virtual void close();
      
    private:
      void *_handle;
    };
  }
}

#endif