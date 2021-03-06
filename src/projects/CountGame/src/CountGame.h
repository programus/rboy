#ifndef __COUNT_GAME_H
#define __COUNT_GAME_H

#include "RboyGame.h"

#define MAX_DISP_NUM     100

#define LIMIT       3000L
#define SPD_UNIT    70

class CountGame : public RboyGame {
private:
  int16_t offx;
  int16_t offy;
  uint16_t fw;
  uint16_t fh;
protected:
  const long MAX_NUM = MAX_DISP_NUM * LIMIT;
  volatile long number = 0;

  void post_init();
  void draw_frame(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, unsigned long interval);
  void handle_button();

  void show_number(int n, long speed=0);
};

#endif
