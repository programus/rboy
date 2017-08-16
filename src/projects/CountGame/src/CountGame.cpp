#include "CountGame.h"

#include <math.h>
#include <Fonts/FreeMonoBold24pt7b.h>

void CountGame::post_init() {
  display.setFont(&FreeMonoBold24pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.getTextBounds("88", 0, 0, &offx, &offy, &fw, &fh);
  fw >>= 1;
}

void CountGame::draw_frame(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, unsigned long interval) {
  static uint8_t prev_disp_num = 0xff;
  double ra = atan2(abs(ay), abs(az));
  double value = (M_PI_4 - ra) * 10;

  long delta = (long) value;
  long speed = delta * (long) interval / 1000;
  number += speed;
  if (number <= 0) {
    number = MAX_NUM - 1;
  }
  number = number % MAX_NUM;
  uint8_t disp_num = (uint8_t) (number / LIMIT);
  show_number(disp_num, speed / SPD_UNIT);
  if (disp_num != prev_disp_num) {
    double freq = 440 * pow(2, (disp_num - 46.0) / 17.0);
    tone((unsigned int) freq, 500);
    prev_disp_num = disp_num;
  }

  // display.setCursor(0, 0);
  // display.setTextSize(1);
  // display.print(1000 / dt);
}

void CountGame::handle_button() {
  number = LIMIT >> 1;
}

void CountGame::show_number(int n, long speed) {
  uint16_t x = (LCDW >> 1) - fw;
  uint16_t y = (LCDH - fh) >> 1;
  uint16_t mx = 18;
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

  uint8_t rr = 3;
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
  display.setCursor(x - offx, y - offy);
  if (n < 10) {
    display.print(0);
  }
  display.print(n);
}
