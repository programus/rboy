#ifndef __MAZE_GAME_H
#define __MAZE_GAME_H

#include "RboyGame.h"
#include "Shapes.h"

#define BLOCKW    6
#define BLOCKH    6

#define MW        (LCDW / BLOCKW)
#define MH        (LCDH / BLOCKH)

class MazeGame : public RboyGame {
private:
  uint8_t maze[(MW + 1) >> 1][(MH + 1) >> 1];
  Point<int8_t> lt = {(LCDW % BLOCKW) >> 1, (LCDH % BLOCKH) >> 1};
  Point<int8_t> solver;
  Point<int8_t> goal;
  uint8_t over_dir;

  void restart();
  void generate_maze();
  void update(int16_t ax, int16_t ay, unsigned long interval);
  void draw();
  void draw_maze();
  void draw_solver();
  void draw_goal();

protected:
  void post_init();
  void handle_button();
  void draw_frame(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, unsigned long interval);
};

#endif
