#include "RawDataGame.h"

#include <math.h>

void RawDataGame::draw_frame(int16_t* raw, size_t len, unsigned long interval) {
  const char* labels[] = { "ax", "ay", "az", "gx", "gy", "gz" };
  uint8_t x = FONTW * 3;
  uint8_t y = 1;
  uint8_t w = LCDW - x;
  uint8_t h = FONTH - 2;
  int rate = 0x8000 / ((w >> 1) - 1);
  uint8_t x0 = x + (w >> 1);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  for (uint8_t i = 0; i < len; i++) {
    display.println(labels[i]);
    display.drawRect(x, y, w, h, WHITE);
    int8_t len = raw[i] / rate;
    int8_t x1 = x0 + len;
    display.fillRect(min(x0, x1), y + 1, abs(len), h - 2, WHITE);
    y += FONTH;
  }

  display.print("fps:");
  display.println(1000 / interval);

  display.display();
}

void RawDataGame::handle_button() {
}
