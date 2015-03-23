#ifndef _KISS_GRAPHICS_H_
#define _KISS_GRAPHICS_H_

enum GraphicsMode {
  GraphicsModeWindow,
  GraphicsModeMemory,
  GraphicsModeMovie
};

void set_graphics_mode();

void graphics_open(int width, int height);

void graphics_close();

#endif