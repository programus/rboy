#include "RawDataGame.h"

#include <math.h>

const PROGMEM uint16_t melody_freqs[] = { 290, 1, LOOP_TONE };
const PROGMEM uint16_t melody_durations[] = { 400, 50, };

void RawDataGame::post_init() {
  display.setTextColor(WHITE);
  display.setTextSize(1);
}

void RawDataGame::draw_frame(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, unsigned long interval) {
  struct {
    const char* label;
    int16_t     value;
  } data[] = {
    {"ax", ax},
    {"ay", ay},
    {"az", az},
    {"gx", gx},
    {"gy", gy},
    {"gz", gz},
  };
  uint8_t x = FONTW * 3;
  uint8_t y = 1;
  uint8_t w = LCDW - x;
  uint8_t h = FONTH - 2;
  int rate = 0x8000 / ((w >> 1) - 1);
  uint8_t x0 = x + (w >> 1);
  display.setCursor(0, 0);
  for (uint8_t i = 0; i < 6; i++) {
    int16_t value = data[i].value;
    display.println(data[i].label);
    display.drawRect(x, y, w, h, WHITE);
    int8_t len = value / rate;
    int8_t x1 = x0 + len;
    display.fillRect(min(x0, x1), y + 1, abs(len), h - 2, WHITE);
    y += FONTH;
  }

  display.print("fps:");
  display.println(1000000.0 / interval);
}

void RawDataGame::handle_button() {
  if (this->is_playing()) {
    this->stop_playing();
  } else {
    this->play_tones(melody_freqs, melody_durations, false);
  }
}
