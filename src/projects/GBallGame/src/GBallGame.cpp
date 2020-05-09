#include "GBallGame.h"

#define RATE    (4.65224e-7)

void GBallGame::post_init() {
  randomSeed(analogRead(0));
  display.invertDisplay(true);
  ball.r = 3;

  do {
    ball.o.x = random(LCDW - ball.r * 2) + ball.r;
    ball.o.y = random(LCDH - ball.r * 2) + ball.r;
  } while (!valid_pos());

  this->set_frame_rate(15);
}

bool GBallGame::valid_pos() {
  Level& curr = levels[curr_level_index];
  for (uint8_t i = 0; i < curr.count; i++) {
    Rect<uint8_t>& rect = curr.rects[i];
    if (intersects(rect, ball)) {
      return false;
    }
  }

  return true;
}

void GBallGame::handle_button() {
}

void GBallGame::draw_frame(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, unsigned long interval) {
  update(-ax, ay);
  draw();
}

void GBallGame::update(int16_t ax, int16_t ay) {
  speed.x += (double)ax * RATE;
  speed.y += (double)ay * RATE;
  Circle<double> fball;
  fball.o.x = ball.o.x + speed.x;
  fball.o.y = ball.o.y + speed.y;
  fball.r = ball.r;

  Level& curr = levels[curr_level_index];
  uint8_t ins = 0;
  for (uint8_t i = 0; i < curr.count; i++) {
    Rect<uint8_t>& rect = curr.rects[i];
    ins |= intersects(rect, fball);
  }
  if (fball.o.x - fball.r < 0 || fball.o.x + fball.r >= LCDW) {
    ins |= 1;
  }
  if (fball.o.y - fball.r < 0 || fball.o.y + fball.r >= LCDH) {
    ins |= 2;
  }

  if (ins & 1) {
    // x
    speed.x = -speed.x;
  } else {
    ball.o.x = fball.o.x;
  }

  if (ins & 2) {
    // y
    speed.y = -speed.y;
  } else {
    ball.o.y = fball.o.y;
  }
}

void GBallGame::draw() {
  // draw background
  Level& curr = levels[curr_level_index];

  for (uint8_t i = 0; i < curr.count; i++) {
    Rect<uint8_t>& rect = curr.rects[i];
    rect.fill(display, WHITE);
  }

  // draw ball
  ball.fill(display, WHITE);

  display.setCursor(0, 0);
  display.print(speed.x);
  display.print(",");
  display.print(speed.y);
}
