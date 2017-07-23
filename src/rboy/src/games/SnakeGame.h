#ifndef __SNAKE_GAME_H
#define __SNAKE_GAME_H

#include "RboyGame.h"

class SnakeGame : public RboyGame {
private:
  const static uint8_t LEN = 50;
  uint8_t body[LEN] = {0};
  uint8_t body_len = 1;
  int8_t head_index = 0;

  uint8_t size;

  // // the values are 256 times of the real values
  // int16_t xh256t;
  // int16_t yh256t;
  uint8_t speed;
  uint8_t direction;
  unsigned long duration;

  int8_t x0;
  int8_t y0;

  uint8_t apple;

  volatile bool running = false;
  volatile bool over = false;

  uint8_t tm;

  uint8_t index(int8_t i);
  uint8_t random_position();
  void restart(bool pause);
  bool move(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, unsigned long interval);
  void drop_apple();
  bool is_in_body(uint8_t data, uint8_t from=0);
  bool is_apple_aten(bool only_head=true);
  void get_speed_direction(int16_t ax, int16_t ay);
  bool is_dead();
  void draw();

protected:
  void post_init();
  void handle_button();
  void draw_frame(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, unsigned long interval);
};

#endif
