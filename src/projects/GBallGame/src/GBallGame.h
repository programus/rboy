#ifndef __G_BALL_GAME_H
#define __G_BALL_GAME_H

#include "RboyGame.h"
#include "Shapes.h"

struct Level {
  Rect<uint8_t>* rects;
  uint8_t count;
};

class GBallGame: public RboyGame {
private:
  Circle<double> ball;
  Point<double> speed;
  Rect<uint8_t> all_rects[6] = {
    {{22, 8}, {5, 43}},
    {{45, 8}, {21, 5}},
    {{61, 13}, {5, 14}},
    {{45, 22}, {16, 5}},
    {{45, 27}, {5, 22}},
    {{50, 44}, {16, 5}},
  };
  Level levels[1] = {
    {all_rects, 6},
  };

  uint8_t curr_level_index;

  bool valid_pos();
  void update(int16_t ax, int16_t ay);
  void draw();

protected:
  void post_init();
  void handle_button();
  void draw_frame(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, unsigned long interval);
};

#endif
