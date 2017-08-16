#ifndef __RBOY_GAME_H
#define __RBOY_GAME_H

#include <Adafruit_SSD1306.h>

#include <I2Cdev.h>
#include <MPU6050.h>

#define LCDW        SSD1306_LCDWIDTH
#define LCDH        SSD1306_LCDHEIGHT
#define FONTW       6
#define FONTH       8

#define ROTATION_0  0
#define ROTATION_1  1
#define ROTATION_2  2
#define ROTATION_3  3

// coordination direction: z-neg(1bit), y-neg(1bit), x-neg(1bit), z-index(2bit), y-index(2bit), x-index(2bit)
// F: front, B: back, L: left, R: right, U: up, D: down. Just like Rubik Cube formula
#define CD_RXBYUZ   0b000100100
#define CD_FXRYUZ   0b001100001
#define CD_LXFYUZ   0b011100100
#define CD_BXLYUZ   0b010100001

#define CD_UXBYLZ   0b100000110
#define CD_UXRYBZ   0b000010010
#define CD_UXFYRZ   0b010000110
#define CD_UXLYFZ   0b110010010

#define CD_RXDYBZ   0b010011000
#define CD_FXDYRZ   0b011001001
#define CD_LXDYFZ   0b111011000
#define CD_BXDYLZ   0b110001001

#define CD_LXBYDZ   0b101100100
#define CD_BXRYDZ   0b100100001
#define CD_RXFYDZ   0b110100100
#define CD_FXLYDZ   0b111100001

#define CD_DXBYRZ   0b001000110
#define CD_DXRYFZ   0b101010010
#define CD_DXFYLZ   0b111000110
#define CD_DXLYBZ   0b011010010

#define CD_RXUYFZ   0b100011000
#define CD_FXUYLZ   0b101001001
#define CD_LXUYBZ   0b001011000
#define CD_BXUYRZ   0b000001001

class RboyGame {
private:
  static RboyGame* attached_game;
  static void button_handler();

protected:
  Adafruit_SSD1306 display;
  MPU6050 mpu;
  unsigned long prev = 0;
  bool mpu_test;
  uint16_t coordination_direction = CD_RXBYUZ;
  unsigned long frame_interval = 0;
  bool need_to_redraw = true;
  uint8_t tone_pin = -1;

  void tone(unsigned int frequency, unsigned long duration = 0);
  void noTone();
  virtual int handle_button_mode();
  virtual void handle_button();
  virtual void post_init();
  virtual void loop(int16_t* raw, size_t len, unsigned long interval);
  virtual void loop(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, unsigned long interval);
  virtual void draw_frame(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, unsigned long interval);

public:
  void attach_tone(uint8_t pin);
  void set_frame_rate(uint8_t fps);
  void set_rotation(uint8_t rotation);
  void set_coordiation_direction(uint16_t coordination_direction);
  bool initialize(uint32_t i2c_clock = 400000);
  void calibrate(int16_t* offsets);
  void calibrate(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz);
  void loop();
  void attach_button(uint8_t pin);
};

#endif
