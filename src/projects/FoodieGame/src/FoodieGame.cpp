#include "FoodieGame.h"
#include "pitches.h"

#include <math.h>

// rules
#define KEEP_GOOD_FOOD

#define MIN_SIZE      4
#define MAX_SIZE      LCDH
#define GROW_SIZE     1

#define FOOD_SIZE     5
#define FOOD_DURATION 5000000

#define DOOR_DURATION 10000

#define ACC_RATE      0.0000000005D

const uint16_t win_freqs[] PROGMEM = { NOTE_G4, 5, NOTE_G4, 5, NOTE_G4, 5, NOTE_G4, NOTE_E4, NOTE_A4, NOTE_G4, END_TONE };
const uint16_t win_duratinos[] PROGMEM = {300, 1, 10, 1, 10, 1, 100, 100, 100, 300};

void FoodieGame::post_init() {
  randomSeed(analogRead(0));
  display.setTextColor(WHITE);
  display.setTextSize(1);
  food.size.x = food.size.y = FOOD_SIZE;
  restart();
}

void FoodieGame::restart() {
  body.set_center(LCDW >> 1, LCDH >> 1);
  body.size.x = body.size.y = MIN_SIZE;
  door_height = LCDH;
  generate_food();
}

void FoodieGame::random_position(Point<uint8_t>* p) {
  p->x = random(LCDW - FOOD_SIZE);
  p->y = random(LCDH - FOOD_SIZE);
}

void FoodieGame::generate_food() {
  do {
    random_position(&(food.origin));
  } while(is_food_aten());
  food_type = body.size.x < random(MIN_SIZE, MAX_SIZE + ((MAX_SIZE - MIN_SIZE) >> 1)) ? GOOD : BAD;
  food_remain = FOOD_DURATION;
}

bool FoodieGame::is_food_aten() {
  return !(
    body.left() > food.right() || 
    body.right() < food.left() ||
    body.top() > food.bottom() ||
    body.bottom() < food.top()
  );
}

void FoodieGame::draw() {
  display.invertDisplay(!running);
  draw_body();
  draw_food();
  draw_message();
}

void FoodieGame::draw_body() {
  body.fill(display, WHITE);
}

void FoodieGame::draw_food() {
  bool draw_food = true;
  if (food_remain < FOOD_DURATION >> 1) {
    draw_food = (food_remain / 500000) & 1;
  }
  if (draw_food || !running) {
    switch(food_type) {
    case GOOD:
      food.draw(display, WHITE, FOOD_SIZE >> 1);
      break;
    case BAD:
      food.draw_cross(display, WHITE);
      break;
    default:
      break;
    }
  }
}

void FoodieGame::draw_message() {
  if (is_won()) {
    display.fillRect(0, 0, LCDW, door_height, BLACK);
    display.fillRect(0, LCDH - door_height, LCDW, LCDH, BLACK);
  }
  if (!running && door_height > LCDH >> 1) {
    uint8_t font_size = 1;
    const char* msg = is_won() ? "WIN!" : "PAUSED";
    uint8_t len = strlen(msg);
    uint8_t w = FONTW * font_size * len;
    uint8_t h = FONTH * font_size;
    uint8_t x = (LCDW - w) >> 1;
    uint8_t y = (LCDH - h) >> 1;
    display.fillRect(x - 5, y - 5, w + 10, h + 10, WHITE);
    display.drawRect(x - 3, y - 3, w + 6, h + 6, BLACK);
    display.setCursor(x, y);
    display.setTextColor(BLACK);
    display.setTextSize(font_size);
    display.print(msg);
  }
}

void FoodieGame::draw_frame(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, unsigned long interval) {
  if (running) {
    calculate(-ax, ay, az, -gx, gy, gz, interval);
  } else if (is_won() && door_height < LCDH) {
    door_remain -= interval;
    if (door_remain > DOOR_DURATION) {
      door_height++;
      door_remain = DOOR_DURATION;
    }
  }
  draw();
}

void FoodieGame::calculate(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, unsigned long interval) {
  // calculate speed
  speed.x += ax * ACC_RATE * interval;
  speed.y += ay * ACC_RATE * interval;
  // move
  body.origin.x += speed.x;
  body.origin.y += speed.y;

  if (body.left() < 0) {
    body.origin.x = 0;
    speed.x = 0;
  }
  if (body.right() + 1 >= LCDW) {
    body.origin.x = LCDW - body.size.x;
    speed.x = 0;
  }
  if (body.top() < 0) {
    body.origin.y = 0;
    speed.y = 0;
  }
  if (body.bottom() + 1 >= LCDH) {
    body.origin.y = LCDH - body.size.y;
    speed.y = 0;
  }
  // food
#ifdef KEEP_GOOD_FOOD
  if (food_type == BAD) {
    food_remain -= interval;
  }
#else
  food_remain -= interval;
#endif
  eat_food();
  if (food_remain > FOOD_DURATION) {
    generate_food();
  }
}

void FoodieGame::eat_food() {
  if (is_food_aten()) {
    if (food_type == GOOD) {
      tone(NOTE_B5, 100);
      body.enlarge(GROW_SIZE, GROW_SIZE);
    } else {
      tone(NOTE_CS2, 100);
      double size = body.size.x;
      body.resize_center(size / 2, size / 2);
    }
    if (is_won()) {
      running = false;
      door_height = 0;
      door_remain = DOOR_DURATION;
      play_tones(win_freqs, win_duratinos, false);
    } else {
      generate_food();
    }
  }
}

bool FoodieGame::is_won() {
  return body.size.y > MAX_SIZE;
}

void FoodieGame::handle_button() {
  if (is_won()) {
    restart();
  } else {
    running = !running;
  }
}
