#include "sdl_display.hpp"
#include <SDL2/SDL.h>
#include <signal.h>

using namespace kiss::graphics;

namespace
{
  struct SdlHandle
  {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
  };
}

SdlDisplay::SdlDisplay()
  : _handle(0)
{
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);
  signal(SIGINT, SIG_DFL);
}

SdlDisplay::~SdlDisplay()
{
  SDL_Quit();
}

bool SdlDisplay::open(const unsigned width, const unsigned height)
{
  SDL_Window *window = SDL_CreateWindow("KISS Graphics", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
  if(!window) return false;
  
  SdlHandle *sdl = new SdlHandle;
  
  sdl->window = window;
  sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED);
  sdl->texture = SDL_CreateTexture(sdl->renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width, height);
  
  _handle = reinterpret_cast<void *>(sdl);
  
  return true;
}

void SdlDisplay::update(const Context &context)
{
  if(!_handle) return;
  SdlHandle *const sdl = reinterpret_cast<SdlHandle *>(_handle);
  
  Context::Rgb *pixels = 0;
  int pitch = 0;
  SDL_LockTexture(sdl->texture, 0, reinterpret_cast<void **>(&pixels), &pitch);
  memcpy(pixels, context.backing(), context.size() * sizeof(Context::Rgb));
  SDL_UnlockTexture(sdl->texture);
  
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
  }
  SDL_RenderCopy(sdl->renderer, sdl->texture, 0, 0);
  SDL_RenderPresent(sdl->renderer);
}

void SdlDisplay::close()
{
  if(!_handle) return;
  SdlHandle *const sdl = reinterpret_cast<SdlHandle *>(_handle);
  SDL_DestroyTexture(sdl->texture);
  SDL_DestroyRenderer(sdl->renderer);
  SDL_DestroyWindow(sdl->window);
  delete sdl;
  _handle = 0;
}