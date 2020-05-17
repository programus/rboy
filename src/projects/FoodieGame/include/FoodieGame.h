#ifndef __RAW_DATA_GAME_H
#define __RAW_DATA_GAME_H

#include "RboyGame.h"
#include "Shapes.h"

enum FoodType {
  GOOD,
  BAD,
  FOOD_TYPE_COUNT
};

class FoodieGame : public RboyGame {
protected:
  Rect<double> body;
  Point<double> speed;

  Rect<uint8_t> food;
  FoodType food_type;
  unsigned long food_remain;

  bool running;
  int16_t door_height;
  unsigned long door_remain;

  void restart();
  void random_position(Point<uint8_t>* p);
  void generate_food();
  void eat_food();
  bool is_food_aten();
  bool is_won();
  void calculate(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, unsigned long interval);
  void draw();
  void draw_message();
  void draw_food();
  void draw_body();

  void post_init();
  void draw_frame(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, unsigned long interval);
  void handle_button();
};

#endif
