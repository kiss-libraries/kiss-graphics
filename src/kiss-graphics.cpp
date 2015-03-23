#include <kiss-graphics/kiss-graphics.hpp>

#include <algorithm>
#include <iostream>
#include <cmath>

using namespace kiss::graphics;

#define FIXED_INTERSECTION_SIZE 64
#define PI (3.1415926f)
#define TAU (2.0f * PI)

Context::Rgb::Rgb() : r(0), g(0), b(0) {}
Context::Rgb::Rgb(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}

Context::Point::Point() : x(0), y(0) {}
Context::Point::Point(const Point::index x, const Point::index y) : x(x), y(y) {}

Context::Line::Line(const Point &a, const Point &b)
{
  points[0] = a;
  points[1] = b;
}

Context::Triangle::Triangle(const Point &a, const Point &b, const Point &c)
{
  points[0] = a;
  points[1] = b;
  points[2] = c;
}

Context::Rectangle::Rectangle(const Point &topLeft, const Point &bottomRight)
{
  points[0] = topLeft;
  points[1] = Point(bottomRight.x, topLeft.y);
  points[2] = bottomRight;
  points[3] = Point(topLeft.x, bottomRight.y);
};

Context::Circle::Circle(const Point &center, const unsigned radius)
{
  for(unsigned i = 0; i < count; ++i)
  {
    const float rad = TAU / count * i;
    
    int x = center.x + cosf(rad) * radius;
    int y = center.y + sinf(rad) * radius;
    
    if(x < 0) x = 0;
    if(y < 0) y = 0;
    
    points[i] = Point(x, y);
  }
}

Context::Context(const unsigned width, const unsigned height)
  : _width(width)
  , _height(height)
  , _backing(new Rgb[size()])
{
}

Context::~Context()
{
  delete[] _backing;
}

void Context::fill(const Rgb &color)
{
  for(unsigned x = 0; x < _width; ++x)
  {
    for(unsigned y = 0; y < _height; ++y) drawUnsafe(Point(x, y), color);
  }
}

void Context::draw(const Point &point, const Rgb &color)
{
  if(point.x > _width || point.y > _height)
  {
    // TODO: Error
    return;
  }
  
  drawUnsafe(point, color);
}

void Context::drawUnsafe(const Point &point, const Rgb &color)
{
  _backing[point.x + point.y * _width] = color;
}

void Context::draw(const Polygon &polygon, const Rgb &color)
{
  using namespace std;

  Point::index top = _height - 1;
  Point::index bottom = 0;
  
  Point::index left = _width - 1;
  Point::index right = 0;
  
  for(unsigned i = 0; i < polygon.count; ++i)
  {
    const Point &p = polygon.points[i];
    
    if(p.y < top) top = p.y;
    if(p.x < left) left = p.x;
    if(p.y > bottom) bottom = p.y;
    if(p.x > right) right = p.x;
  }
  
  if(bottom >= _height) bottom = _height - 1;
  
  for(unsigned scan = top; scan <= bottom; ++scan)
  {
    for(unsigned i = 0; i < polygon.count; ++i)
    {
      Point p[2] = {
        polygon.points[i],
        polygon.points[(i + 1) % polygon.count]
      };
      
      if((p[0].y < scan && p[1].y < scan) || (p[0].y > scan && p[1].y > scan)) continue;
      
      if(p[0].x == p[1].x)
      {
        drawUnsafe(Point(p[0].x, scan), color);
        continue;
      }
      
      if(p[0].y == p[1].y) continue;
      if(p[0].x > p[1].x) swap(p[0], p[1]);
      
      const int dy = static_cast<int>(p[1].y) - static_cast<int>(p[0].y);
      const unsigned dx = p[1].x - p[0].x;
      const float m = static_cast<float>(dy) / static_cast<float>(dx);
      const int intersection = p[0].x + (static_cast<int>(scan) - static_cast<int>(p[0].y)) / m;
      if(intersection < 0 || intersection >= _width) continue;
      
      drawUnsafe(Point(intersection, scan), color);
    }
  }
  
  for(unsigned scan = left; scan <= right; ++scan)
  {
    for(unsigned i = 0; i < polygon.count; ++i)
    {
      Point p[2] = {
        polygon.points[i],
        polygon.points[(i + 1) % polygon.count]
      };
      
      if((p[0].x < scan && p[1].x < scan) || (p[0].x > scan && p[1].x > scan)) continue;
      
      if(p[0].y == p[1].y)
      {
        drawUnsafe(Point(scan, p[0].y), color);
        continue;
      }
      
      if(p[0].x == p[1].x) continue;
      if(p[0].y > p[1].y) swap(p[0], p[1]);
      
      const unsigned dy = p[1].y - p[0].y;
      const int dx = static_cast<int>(p[1].x) - static_cast<int>(p[0].x);
      const float m = static_cast<float>(dx) / static_cast<float>(dy);
      const int intersection = p[0].y + (static_cast<int>(scan) - static_cast<int>(p[0].x)) / m;
      if(intersection < 0 || intersection >= _height) continue;
      
      drawUnsafe(Point(scan, intersection), color);
    }
  }
}

void Context::fill(const Polygon &polygon, const Rgb &color)
{
  using namespace std;

  unsigned top = _height - 1;
  unsigned bottom = 0;
  
  for(unsigned i = 0; i < polygon.count; ++i)
  {
    const Point &p = polygon.points[i];
    
    if(p.y < top) top = p.y;
    if(p.y > bottom) bottom = p.y;
  }
  
  if(bottom > _height) bottom = _height;
  
  unsigned fixed_intersections[FIXED_INTERSECTION_SIZE];
  
  unsigned *intersections = fixed_intersections;
  bool fixed = true;
  
  if(polygon.count > FIXED_INTERSECTION_SIZE)
  {
    intersections = new unsigned[polygon.count];
    fixed = false;
  }
  
  for(unsigned scan = top; scan < bottom; ++scan)
  {
    unsigned numIntersections = 0;
    
    for(unsigned i = 0; i < polygon.count; ++i)
    {
      Point p[2] = {
        polygon.points[i],
        polygon.points[(i + 1) % polygon.count]
      };
      
      if((p[0].y < scan && p[1].y < scan) || (p[0].y > scan && p[1].y > scan)) continue;
      
      if(p[0].x == p[1].x)
      {
        intersections[numIntersections++] = p[0].x;
        continue;
      }
      
      if(p[0].y == p[1].y) continue;
      if(p[0].x > p[1].x) swap(p[0], p[1]);
      
      const int dy = static_cast<int>(p[1].y) - static_cast<int>(p[0].y);
      const unsigned dx = p[1].x - p[0].x;
      const float m = static_cast<float>(dy) / static_cast<float>(dx);
      const int intersection = p[0].x + (static_cast<int>(scan) - static_cast<int>(p[0].y)) / m;
      if(intersection < 0 || intersection >= _width) continue;
      
      intersections[numIntersections++] = intersection;
    }
    
    if(numIntersections < 2) continue;
    
    sort(intersections, intersections + numIntersections);
    numIntersections = unique(intersections, intersections + numIntersections) - intersections;
    
    for(unsigned i = 0; i < numIntersections; i += 2)
    {
      unsigned s = intersections[i];
      unsigned e = intersections[i + 1];
      for(unsigned x = s; x <= e; ++x) drawUnsafe(Point(x, scan), color);
    }
  }
  
  if(!fixed) delete[] intersections;
  
  // Final pass. Handle horizontal lines.
  for(unsigned i = 0; i < polygon.count; ++i)
  {
    Point p[2] = {
      polygon.points[i],
      polygon.points[(i + 1) % polygon.count]
    };
    
    if(p[0].y != p[1].y) continue;
    if(p[0].x > p[1].x) swap(p[0], p[1]);
    
    for(unsigned x = p[0].x; x < p[1].x; ++x) drawUnsafe(Point(x, p[0].y), color);
  }
}