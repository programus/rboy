#include "CountGame.h"

#include <math.h>

void CountGame::draw_frame(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, unsigned long interval) {
  double ayz = sqrt(sq((unsigned long)ay) + sq((unsigned long)az));
  double sinv = max(-1.0, min(1.0, ayz / ((double) 0x3fff)));
  double value = (M_PI_4 - abs(asin(sinv))) * 10;

  long delta = (long) value;
  long dt = interval;
  long speed = delta * dt;
  number += speed;
  if (number <= 0) {
    number = MAX_NUM - 1;
  }
  number = number % MAX_NUM;
  display.clearDisplay();
  show_number(number / LIMIT, speed / SPD_UNIT);

  // display.setCursor(0, 0);
  // display.setTextSize(1);
  // display.print(1000 / dt);

  display.display();
}

void CountGame::handle_button() {
  number = LIMIT >> 1;
}

void CountGame::show_number(int n, long speed, uint8_t size) {
  uint16_t fw = FONTW * size;
  uint16_t fh = FONTH * size;
  uint16_t x = (LCDW >> 1) - fw;
  uint16_t y = (LCDH - fh) >> 1;
  uint16_t mx = 16;
  uint16_t my = 6;
  uint8_t frame_count = 2;
  // draw frames
  for (uint8_t i = 0; i < frame_count; i++) {
    int offset = i << 1;
    display.drawRect(x - mx - 1 - offset, y - my - 1 - offset, (fw << 1) + (mx << 1) + (offset << 1), fh + (my << 1) + (offset << 1), WHITE);
  }

  // draw speed indicators
  uint8_t sx[2];
  sx[0] = (x - mx - 1 - ((frame_count) << 1)) >> 1;
  sx[1] = LCDW - sx[0];
  uint8_t sy = LCDH >> 1;

  uint8_t rr = sx[0] >> 1;
  for (uint8_t i = 0; i < sizeof(sx); i++) {
    int8_t ssx = sx[i];
    int8_t padding = 3;
    int8_t space = 4;
    display.fillCircle(ssx, sy, rr >> 1, WHITE);
    for (uint8_t j = 1; j < abs(speed) + 1; j++) {
      int8_t offset = j * space + padding;
      int8_t ssy = sy + (speed > 0 ? -offset : offset);
      int8_t d = rr;
      int8_t dx = d;
      int8_t dy = speed > 0 ? d : -d;
      display.drawLine(ssx, ssy, ssx - dx, ssy + dy, WHITE);
      display.drawLine(ssx, ssy, ssx + dx, ssy + dy, WHITE);
    }
  }

  // draw number
  if (n < 10) {
    x += fw;
  }
  display.setTextSize(size);
  display.setTextColor(WHITE);
  display.setCursor(x, y);
  display.print(n);
}
