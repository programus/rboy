#include "MazeGame.h"

#define W               0b00
#define S               0b01
#define E               0b10
#define N               0b11
#define DC              4

#define OPPOSITE(d)     (((d) + 2) & 0b11)

#define DX(i)           ((((i) >> 1) ^ ((i) & 1)) ? ((i) >> 1) : (int8_t)((i) & 1) - 1)
#define DY(i)           ((((i) >> 1) ^ ((i) & 1)) ? ((i) & 1) : 0 - (int8_t)((i) >> 1))
#define M(m, x, y)      (((m[(x) >> 1][(y) >> 1] >> (((x) & 1) << 1)) >> (((y) & 1) << 2)) & 0b11)
#define MC(m, x, y)     (M(m, x, y) || (M(m, x - 1, y) & E) || (M(m, x, y - 1) & S))
#define MS(m, x, y, v)  m[(x) >> 1][(y) >> 1] |= ((v << (((x) & 1) << 1)) << (((y) & 1) << 2))
#define DIR(v)          ((v) & 0b11) == 0 ? \
                          (1 + 2 + 3 - (((v) >> 2) & 0b11) - (((v) >> 4) & 0b11) - (((v) >> 6) & 0b11)) : \
                          ((v) >> (((v) & 0b11) << 1)) & 0b11
#define INCI(v)         v = (v & 0b11111100) | ((v & 0b11) + 1)


void MazeGame::restart() {
  solver = {(int8_t) (random(MW) * BLOCKW), (int8_t) (random(MH) * BLOCKH)};
  goal.x = solver.x > BLOCKW * (MW >> 1) ? 0 : MW - 1;
  goal.y = solver.y > BLOCKH * (MH >> 1) ? 0 : MH - 1;
  memset(maze, 0, sizeof(maze));
  generate_maze();
  over_dir = 0;
  over_dir |= random(DC) & 0b11;
}

void MazeGame::generate_maze() {
  Point p = {0, 0};
  uint8_t stack[MW * MH];
  size_t index = 0;
  uint8_t v;
  bool init_block = true;
  bool jump_in = false;
  bool jump_out = false;
  do {
    if (init_block) {
      v = 0b11100100;
      for (uint8_t i = DC - 1; i > 0; i--) {
        uint8_t j = random(i + 1);
        uint8_t t = ((v >> (i << 1)) & 0b11) << (j << 1);
        t |= ((v >> (j << 1)) & 0b11) << (i << 1);
        v &= ~((0b11 << (i << 1)) | (0b11 << (j << 1)));
        v |= t;
      }
      v &= 0b11111100;
    }

    do {
      uint8_t dir;
      Point np;
      if (jump_out) {
        dir = OPPOSITE(DIR(v));
        np.x = p.x + DX(dir);
        np.y = p.y + DY(dir);
        p.x = np.x;
        p.y = np.y;
        INCI(v);
        jump_out = false;
      }
      dir = DIR(v);
      np = {(int8_t)(p.x + DX(dir)), (int8_t)(p.y + DY(dir))};
      if (np.x >= 0 && np.x < MW && np.y >= 0 && np.y < MH && !MC(maze, np.x, np.y)) {
        switch (dir) {
        case 0b00:
          MS(maze, p.x - 1, p.y, E);
          break;
        case 0b01:
        case 0b10:
          MS(maze, p.x, p.y, dir);
          break;
        case 0b11:
          MS(maze, p.x, p.y - 1, S);
          break;
        }
        stack[index++] = v;
        p.x = np.x;
        p.y = np.y;
        init_block = true;
        jump_in = true;
        jump_out = false;
        break;
      }
      INCI(v);
      jump_in = false;
      jump_out = false;
    } while ((v & 0b11) != 0);

    if (!jump_in) {
      v = stack[--index];
      jump_out = true;
      init_block = false;
    }

  } while (index != 0);
}

void MazeGame::update(int16_t ax, int16_t ay, unsigned long interval) {
  Point block = {(int8_t)(solver.x / BLOCKW), (int8_t)(solver.y / BLOCKH)};
  uint8_t walls = 0;
  if (block.y >= MH - 1 || (M(maze, block.x, block.y) & S) == 0) {
    walls |= (1 << S);
  }
  if (block.y <= 0 || (M(maze, block.x, block.y - 1) & S) == 0) {
    walls |= (1 << N);
  }
  if (block.x >= MW - 1 || (M(maze, block.x, block.y) & E) == 0) {
    walls |= (1 << E);
  }
  if (block.x <= 0 || (M(maze, block.x - 1, block.y) & E) == 0) {
    walls |= (1 << W);
  }

  int8_t dx = (ax >> 8) * (BLOCKW - 1) / 0x40;
  int8_t dy = (ay >> 8) * (BLOCKH - 1) / 0x40;

  if (abs(dx) > abs(dy)) {
    solver.x += dx;
    dy = 0;
    over_dir &= 0xf0;
    over_dir |= dx > 0 ? E : W;
  } else {
    solver.y += dy;
    dx = 0;
    if (dy != 0) {
      over_dir &= 0xf0;
      over_dir |= dy > 0 ? S : N;
    }
  }

  if (dy > 0 && solver.y > block.y * BLOCKH && ((walls & (1 << S)) || (solver.x > block.x * BLOCKW && (block.y + 1 > MH - 1 || (M(maze, block.x, block.y + 1) & E) == 0 || (M(maze, block.x + 1, block.y) & S) == 0)))) {
    solver.y = block.y * BLOCKH;
  }
  if (dy < 0 && solver.y < block.y * BLOCKH && ((walls & (1 << N)) || (solver.x > block.x * BLOCKW && (block.y - 1 < 0 || (M(maze, block.x, block.y - 1) & E) == 0 || (M(maze, block.x + 1, block.y - 1) & S) == 0)))) {
    solver.y = block.y * BLOCKH;
  }
  if (dx > 0 && solver.x > block.x * BLOCKW && ((walls & (1 << E)) || (solver.y > block.y * BLOCKH && (block.x + 1 > MW - 1 || (M(maze, block.x + 1, block.y) & S) == 0 || (M(maze, block.x, block.y + 1) & E) == 0)))) {
    solver.x = block.x * BLOCKW;
  }
  if (dx < 0 && solver.x < block.x * BLOCKW && ((walls & (1 << W)) || (solver.y > block.y * BLOCKH && (block.x - 1 < 0 || (M(maze, block.x - 1, block.y) & S) == 0 || (M(maze, block.x - 1, block.y + 1) & E) == 0)))) {
    solver.x = block.x * BLOCKW;
  }
}

void MazeGame::draw() {
  draw_maze();
  draw_solver();
  draw_goal();
}

void MazeGame::draw_maze() {
  display.fillRect(lt.x + 1, lt.y + 1, BLOCKW * MW, BLOCKH * MH, WHITE);
  for (uint8_t row = 0; row < MW; row++) {
    for (uint8_t col = 0; col < MH; col++) {
      uint8_t data = M(maze, row, col);
      if (!(data & S)) {
        int x = lt.x + BLOCKW * row;
        int y = lt.y + BLOCKH * col + BLOCKH;
        display.drawLine(x, y, x + BLOCKW, y, BLACK);
      }
      if (!(data & E)) {
        int x = lt.x + BLOCKW * row + BLOCKW;
        int y = lt.y + BLOCKH * col;
        display.drawLine(x, y, x, y + BLOCKH, BLACK);
      }
    }
  }
}

void MazeGame::draw_solver() {
  Point center = {(int8_t)(lt.x + solver.x + (BLOCKW >> 1)), (int8_t)(lt.y + solver.y + (BLOCKH >> 1))};
  int8_t dx = DX(over_dir & 0b11);
  int8_t dy = DY(over_dir & 0b11);

  display.drawLine(center.x - 2, center.y, center.x + 2, center.y, BLACK);
  display.drawLine(center.x, center.y - 2, center.x, center.y + 2, BLACK);
  display.drawLine(
    center.x - (dx == 0 ? 1 : (dx << 1)), center.y - (dy == 0 ? 1 : (dy << 1)),
    center.x + (dx == 0 ? 1 : -(dx << 1)), center.y + (dy == 0 ? 1 : -(dy << 1)),
    BLACK);
  display.fillRect(center.x - 1 + (dx > 0), center.y - 1 + (dy > 0), 3 - abs(dx), 3 - abs(dy), BLACK);
}

void MazeGame::draw_goal() {
  Point center = {(int8_t)(lt.x + goal.x * BLOCKW + (BLOCKW >> 1)), (int8_t)(lt.y + goal.y * BLOCKH + (BLOCKH >> 1))};

  display.drawLine(center.x - 2, center.y, center.x + 2, center.y, BLACK);
  display.drawLine(center.x, center.y - 2, center.x, center.y + 2, BLACK);
  // display.fillRect(center.x - 1, center.y - 1, 3, 3, BLACK);
  display.drawPixel(center.x, center.y, WHITE);
}

void MazeGame::post_init() {
  randomSeed(analogRead(0));
  over_dir |= 0x10;
}

void MazeGame::handle_button() {
  over_dir |= 0x10;
}

void MazeGame::draw_frame(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, unsigned long interval) {
  if (over_dir & 0x10) {
    restart();
  } else {
    update(-ax, ay, interval);
  }
  draw();
  if ((solver.x / BLOCKW == goal.x) && (solver.y / BLOCKH == goal.y)) {
    over_dir |= 0x10;
  }
}
