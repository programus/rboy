#ifndef __RAW_DATA_GAME_H
#define __RAW_DATA_GAME_H

#include "RboyGame.h"

class RawDataGame : public RboyGame {
protected:
  void draw_frame(int16_t* raw, size_t len, unsigned long interval);
  void handle_button();
};

#endif
