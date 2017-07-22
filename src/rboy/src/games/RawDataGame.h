#ifndef __RAW_DATA_GAME_H
#define __RAW_DATA_GAME_H

#include "RboyGame.h"

class RawDataGame : public RboyGame {
protected:
  void post_init();
  void draw_frame(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, unsigned long interval);
  void handle_button();
};

#endif
