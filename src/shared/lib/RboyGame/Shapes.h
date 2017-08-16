#ifndef __SHAPES_H
#define __SHAPES_H

#include <Adafruit_SSD1306.h>

template <typename T>
struct Point {
  T x;
  T y;
};

template <typename T>
using Size=Point<T>;


template <typename T>
struct Rect {
  Point<T> origin;
  Size<T>  size;

  template <typename U>
  inline bool contains(Point<U>* p) {
    return p->x >= origin.x && p->y >= origin.y && p->x < origin.x + size.x && p->y < origin.y + size.y;
  }

  inline void enlarge(T dx, T dy) {
    this->origin.x -= dx;
    this->origin.y -= dy;
    this->size.x += dx * 2;
    this->size.y += dy * 2;
  }
  inline void enlarge(Size<T>* cs) {
    this->enlarge(cs->x, cs->y);
  }
  inline void resize_center(Size<T>* s) {
    this->origin.x -= (s.x - size.x) / 2;
    this->origin.y -= (s.y - size.y) / 2;
    size.x = s->x;
    size.y = s->y;
  }
  inline void center(Point<T>* center_point) {
    center_point->x = center_x();
    center_point->y = center_y();
  }
  inline T center_x() {
    return origin.x + size.x / 2;
  }
  inline T center_y() {
    return origin.y + size.y / 2;
  }
  inline T left() {
    return origin.x;
  }
  inline T right() {
    return origin.x + size.x - 1;
  }
  inline T top() {
    return origin.y;
  }
  inline T bottom() {
    return origin.y + size.y - 1;
  }
  inline void draw(Adafruit_GFX& display, uint16_t color) {
    display.drawRect((int16_t)origin.x, (int16_t)origin.y, (int16_t)size.x, (int16_t)size.y, color);
  }
  inline void fill(Adafruit_GFX& display, uint16_t color) {
    display.fillRect((int16_t)origin.x, (int16_t)origin.y, (int16_t)size.x, (int16_t)size.y, color);
  }
  inline void draw(Adafruit_GFX& display, uint16_t color, int16_t radius) {
    display.drawRoundRect((int16_t)origin.x, (int16_t)origin.y, (int16_t)size.x, (int16_t)size.y, radius, color);
  }
  inline void fill(Adafruit_GFX& display, uint16_t color, int16_t radius) {
    display.fillRoundRect((int16_t)origin.x, (int16_t)origin.y, (int16_t)size.x, (int16_t)size.y, radius, color);
  }
};

template <typename T>
struct Circle {
  Point<T> o;
  T r;
  inline T left() {
    return o.x - r;
  }
  inline T right() {
    return o.x + r;
  }
  inline T top() {
    return o.y - r;
  }
  inline T bottom() {
    return o.y + r;
  }
  template <typename U>
  inline bool contains(Point<U>* p) {
    auto dx = abs(p->x - o.x);
    if (dx > r) {
      return false;
    }
    auto dy = abs(p->y - o.y);
    if (dy > r) {
      return false;
    }
    if (dx + dy <= r) {
      return true;
    }
    return dx * dx + dy * dy <= r * r;
  }
  inline void draw(Adafruit_GFX& display, uint16_t color) {
    display.drawCircle(o.x, o.y, r, color);
  }
  inline void fill(Adafruit_GFX& display, uint16_t color) {
    display.fillCircle(o.x, o.y, r, color);
  }
};

template<typename T, typename U>
uint8_t intersects(Rect<T>& rect, Circle<U> circle) {
  auto dx = abs(circle.o.x - rect.center_x()) - rect.size.x / 2;
  auto dy = abs(circle.o.y - rect.center_y()) - rect.size.y / 2;

  if (dx > circle.r || dy > circle.r) {
    return 0;
  }

  if (dx <= 0) {
    return 1;
  }

  if (dy <= 0) {
    return 2;
  }

  return dx * dx + dy * dy <= circle.r * circle.r ? 3 : 0;
}

#endif
