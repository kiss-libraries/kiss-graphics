#include <kiss-graphics/kiss-graphics.hpp>
#include <kiss-graphics/socket_display.hpp>
#include <kiss-graphics/sdl_display.hpp>

#include <iostream>

using namespace kiss::graphics;

int main(int argc, char *argv[])
{
  SocketDisplay disp("127.0.0.1", 60000);
  SdlDisplay sdl;
  
  if(!disp.open(800, 600))
  {
    std::cerr << "Failed to open display!" << std::endl;
    return 1;
  }
  
  Context::Circle circle(Context::Point(400, 300), 300);
  Context::Circle circle2(Context::Point(400, 300), 280);
  
  Context c(800, 600);
  c.draw(circle, Context::Rgb(255, 0, 0));
  c.fill(circle2, Context::Rgb(255, 255, 0));
  
  unsigned char i = 0;
  while(1)
  {
    c.fill(circle2, Context::Rgb(255, 255, i++));
    
    disp.update(c);
  }
  
  disp.close();
  
  return 0;
}