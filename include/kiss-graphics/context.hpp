#ifndef _KISS_GRAPHICS_CONTEXT_HPP_
#define _KISS_GRAPHICS_CONTEXT_HPP_

namespace kiss
{
  namespace graphics
  {
    class Context
    {
    public:
      struct __attribute__((packed)) Rgb
      {
        Rgb();
        Rgb(unsigned char r, unsigned char g, unsigned char b);
        
        unsigned char r;
        unsigned char g;
        unsigned char b;
      };
      
      struct Point
      {
        typedef unsigned index;
        
        Point();
        Point(const index x, const index y);
        
        index x;
        index y;
      };
      
      template<unsigned C>
      struct FixedPolygon
      {
        const static unsigned count = C;
        Point points[count];
      };
      
      struct Polygon
      {
        Point *points;
        unsigned count;
      };
      
      struct Line : FixedPolygon<2>
      {
        Line(const Point &a, const Point &b);
      };
      
      struct Triangle : FixedPolygon<3>
      {
        Triangle(const Point &a, const Point &b, const Point &c);
      };
      
      struct Rectangle : FixedPolygon<4>
      {
        Rectangle(const Point &topLeft, const Point &bottomRight);
      };
      
      struct Circle : FixedPolygon<64>
      {
        Circle(const Point &center, const unsigned radius);
        
        Point center;
        unsigned radius;
      };
      
      Context(const unsigned width, const unsigned height);
      ~Context();
    
      void fill(const Rgb &color);
      void draw(const Point &point, const Rgb &color);
      
      template<unsigned count>
      void draw(const FixedPolygon<count> &polygon, const Rgb &color)
      {
        const Polygon surrogate = {
          .points = const_cast<Point *>(polygon.points),
          .count  = count
        };
        
        draw(surrogate, color);
      }
      
      template<unsigned count>
      void fill(const FixedPolygon<count> &polygon, const Rgb &color)
      {
        const Polygon surrogate = {
          .points = const_cast<Point *>(polygon.points),
          .count  = count
        };
        
        fill(surrogate, color);
      }
      
      void draw(const Polygon &polygon, const Rgb &color);
      void fill(const Polygon &polygon, const Rgb &color);
    
      inline const Rgb *backing() const { return _backing; }
    
      inline unsigned width()  const { return _width; }
      inline unsigned height() const { return _height; }
      inline unsigned size()   const { return _width * _height; }
    
    private:
      void drawUnsafe(const Point &point, const Rgb &color);
      
      unsigned _width;
      unsigned _height;
    
      Rgb *_backing;
    };
  }
}

#endif