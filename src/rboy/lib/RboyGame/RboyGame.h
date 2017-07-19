#ifndef __RBOY_GAME_H
#define __RBOY_GAME_H

#include <Adafruit_SSD1306.h>

#include <I2Cdev.h>
#include <MPU6050.h>

#define LCDW        SSD1306_LCDWIDTH
#define LCDH        SSD1306_LCDHEIGHT
#define FONTW       6
#define FONTH       8

class RboyGame {
protected:
  Adafruit_SSD1306 display;
  MPU6050 mpu;
  unsigned long prev = 0;
  bool mpu_test;

  static RboyGame* attached_game;

  static void button_handler();

  virtual void handle_button();
  virtual void draw_frame(int16_t* raw, size_t len, unsigned long interval);
  virtual void draw_frame(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, unsigned long interval);

public:
  bool initialize();
  void calibrate(int16_t* offsets);
  void calibrate(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz);
  void loop();
  void attach_button(int pin, int mode=RISING);
};

#endif
