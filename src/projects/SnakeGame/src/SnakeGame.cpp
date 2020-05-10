#include "SnakeGame.h"
#include <math.h>

#define CELLW       12
#define CELLH       12

#define N_SPEED     1
#define H_SPEED     4
#define N_THRES     (M_PI / 12)
#define H_THRES     (M_PI / 6)
#define MOVE_THRES  1000000L

#define X(d)        ((int8_t)((d) >> 4))
#define Y(d)        ((int8_t)((d) & 0xf))
#define D(x, y)     ((uint8_t)(((x) << 4) | ((y) & 0xf)))
#define DATA(x, y)  D((x) / CELLW, (y) / CELLH)
#define CX(d)       (X(d) * CELLW)
#define CY(d)       (Y(d) * CELLH)

void SnakeGame::handle_button() {
  running = !running;
}

void SnakeGame::post_init() {
  randomSeed(analogRead(0));
  size = D(LCDW / CELLW, LCDH / CELLH);
  x0 = (LCDW % CELLW) >> 1;
  y0 = (LCDH % CELLH) >> 1;
  set_frame_rate(10);
  display.setTextWrap(false);

  tm = D((CELLW >> 1) - FONTW, (CELLH - FONTH) >> 1);
  restart(true);
}

uint8_t SnakeGame::random_position() {
  return D(random(X(size)), random(Y(size)));
}

void SnakeGame::restart(bool pause) {
  over = false;
  running = !pause;
  // xh256t = ((LCDW >> 2) + random(LCDW >> 1)) << 8;
  // yh256t = ((LCDH >> 2) + random(LCDH >> 1)) << 8;
  body_len = 1;
  head_index = 0;
  body[head_index] = D(X(size) >> 1, Y(size) >> 1);
  duration = 0;
  direction = random(4);
  speed = N_SPEED;
  drop_apple();
  if (Serial) {
    Serial.println("rs");
    Serial.println(head_index);
    Serial.println(apple);
    Serial.println(size, HEX);
  }
}

uint8_t SnakeGame::index(int8_t i) {
  for (; i < 0; i+= LEN);
  for (; i >= LEN; i -= LEN);
  return i;
}

void SnakeGame::drop_apple() {
  if (body_len < LEN) {
    do {
      apple = random_position();
    } while(is_apple_aten(false));
  } else {
    // if there are full of snake, no place for apple.
    apple = D(-1, -1);
  }
}

bool SnakeGame::is_in_body(uint8_t data, uint8_t from) {
  uint8_t l = body_len;
  if (l == 2) {
    l = 3;
  }
  for (uint8_t i = from; i < l; i++) {
    uint8_t ii = index(head_index - i);
    if (body[ii] == data) {
      return true;
    }
  }
  return false;
}

bool SnakeGame::is_apple_aten(bool only_head) {
  return only_head ? body[head_index] == apple : is_in_body(apple);
}

void SnakeGame::get_speed_direction(int16_t ax, int16_t ay) {
  int16_t a = max(abs(ax), abs(ay));
  double rad = abs(asin(a / (double)0x4000));
  if (rad > N_THRES) {
    if (abs(ax) > abs(ay)) {
      direction = ax > 0 ? 0b10 : 0b00;
    } else {
      direction = ay > 0 ? 0b11 : 0b01;
    }
    speed = N_SPEED;
    if (rad > H_THRES) {
      speed = H_SPEED;
    }
  }
}

bool SnakeGame::move(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, unsigned long interval) {
  get_speed_direction(-ax, ay);
  bool moved = false;
  duration += interval;
  if (speed * duration > MOVE_THRES) {
    uint8_t data = body[head_index];
    if (direction & 1) {
      // vertical
      data = D(X(data), Y(data) + ((direction >> 1) ? 1 : -1));
    } else {
      // horizontal
      data = D(X(data) + ((direction >> 1) ? 1 : -1), Y(data));
    }
    head_index = index(head_index + 1);
    body[head_index] = data;
    moved = true;
    duration = 0;
  }

  return moved;
}

bool SnakeGame::is_dead() {
  int8_t x = X(body[head_index]);
  int8_t y = Y(body[head_index]);
  return x < 0 || x >= X(size) || y < 0 || y >= Y(size) || is_in_body(body[head_index], 1);
}

void SnakeGame::draw() {
  display.invertDisplay(!running);
  // draw walls
  display.fillRect(0, 0, LCDW, y0, WHITE);
  display.fillRect(0, 0, x0, LCDH, WHITE);
  display.fillRect(LCDW - x0, 0, x0, LCDH, WHITE);
  display.fillRect(0, LCDH - y0, LCDW, y0, WHITE);

  // draw direction
  uint8_t xd0 = CX(body[head_index]) + x0 + (CELLW >> 1);
  uint8_t yd0 = CY(body[head_index]) + y0 + (CELLH >> 1);
  uint8_t xd1 = xd0;
  uint8_t yd1 = yd0;
  uint8_t delta = 2;
  if (direction & 1) {
    yd1 += (direction >> 1) ? (CELLH >> 1) + delta : -((CELLH >> 1) + delta);
  } else {
    xd1 += (direction >> 1) ? (CELLW >> 1) + delta : -((CELLW >> 1) + delta);
  }
  display.drawLine(xd0, yd0, xd1, yd1, WHITE);
  display.drawPixel(xd1 - 1, yd1, WHITE);
  display.drawPixel(xd1 + 1, yd1, WHITE);
  display.drawPixel(xd1, yd1 - 1, WHITE);
  display.drawPixel(xd1, yd1 + 1, WHITE);

  // draw snake
  for (uint8_t i = 1; i <= body_len; i++) {
    uint8_t ii = index(head_index - (i - 1));
    uint8_t x = CX(body[ii]) + x0;
    uint8_t y = CY(body[ii]) + y0;
    display.fillRoundRect(x, y, CELLW, CELLH, 1, WHITE);
    display.setCursor(x + X(tm) + (i < 10 ? (FONTW >> 1) : 0), y + Y(tm));
    display.setTextColor(BLACK);
    display.setTextSize(1);
    display.print(i);
  }

  // draw apple
  uint8_t r = min(CELLW >> 1, CELLH >> 1);
  uint8_t xo = X(apple) * CELLW + x0 + r;
  uint8_t yo = Y(apple) * CELLH + y0 + r;
  display.drawCircle(xo, yo, r - 4, WHITE);
  display.fillRect(xo, yo - r, r, r << 1, BLACK);
  display.fillRect(xo - r, yo, r, r, BLACK);
  display.drawCircle(xo, yo, r - 1, WHITE);

  if (!running) {
    display.drawRect(0, 0, LCDW, LCDH, BLACK);
  }

  draw_message();
}

void SnakeGame::draw_message() {
  if (!running) {
    if (body_len >= LEN) {
      draw_win();
    } else {
      uint8_t font_size = 1;
      const char* msg = over ? "GAME OVER" : "PAUSED";
      uint8_t len = strlen(msg);
      uint8_t w = FONTW * font_size * len;
      uint8_t h = FONTH * font_size;
      uint8_t x = (LCDW - w) >> 1;
      uint8_t y = (LCDH - h) >> 1;
      display.fillRect(x - 5, y - 5, w + 10, h + 10, WHITE);
      display.drawRect(x - 3, y - 3, w + 6, h + 6, BLACK);
      display.setCursor(x, y);
      display.setTextColor(BLACK);
      display.setTextSize(font_size);
      display.print(msg);
    }
  }
}

void SnakeGame::draw_win() {
  static uint8_t full_size = 1;
  static uint8_t count = 0;
  if (body_len >= LEN && over) {
    const char* full_msg = "FULL!";
    uint8_t len = strlen(full_msg);
    uint8_t w = FONTW * full_size * len;
    uint8_t h = FONTH * full_size;
    uint8_t x = (LCDW - w) >> 1;
    uint8_t y = (LCDH - h) >> 1;
    display.fillRect(x - 5, y - 5, w + 10, h + 10, BLACK);
    display.drawRect(x - 3, y - 3, w + 6, h + 6, WHITE);
    display.setCursor(x, y);
    display.setTextColor(WHITE);
    display.setTextSize(full_size);
    display.print(full_msg);
    count++;
    if (count > 5) {
      count = 0;
      if (w + 10 < LCDW && h + 10 < LCDH) {
        full_size++;
      } else {
        full_size--;
      }
    }
  } else {
    full_size = 1;
    count = 0;
  }
}

void SnakeGame::draw_frame(int ax, int ay, int az, int gx, int gy, int gz, unsigned long interval) {
  if (!running) {
    draw();
    display.invertDisplay(!running);
  } else {
    if (over) {
      restart(false);
      need_to_redraw = true;
    }
    if (need_to_redraw) {
      draw();
    }
    bool moved = move(ax, ay, az, gx, gy, gz, interval);
    if (moved) {
      if (is_apple_aten(true)) {
        tone(784, 200);
        drop_apple();
        body_len++;
      }
      if (is_dead()) {
        running = false;
        over = true;
        const static uint16_t tones[] PROGMEM = { 98, 87, 82, 73, 65, 0 };
        const static uint16_t durations[] PROGMEM = { 400, 400, 400, 400, 400 };
        play_tones(tones, durations, false);
      }
    }
    need_to_redraw = true;
  }
}
