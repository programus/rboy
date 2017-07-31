#include "RboyGame.h"
#include <Wire.h>

#define INDEX(x, i)   ((x >> (i << 1)) & 0b11)
#define NEG(x, i)     ((x >> (6 + i)) & 1)

RboyGame* RboyGame::attached_game = NULL;

void RboyGame::set_rotation(uint8_t rotation) {
  display.setRotation(rotation);
}

void RboyGame::set_coordiation_direction(uint16_t coordination_direction) {
  this->coordination_direction = coordination_direction;
}

void RboyGame::set_frame_rate(uint8_t fps) {
  frame_interval = fps > 0 ? 1000000 / fps : 0;
}

bool RboyGame::initialize(uint32_t i2c_clock) {
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.display();
  Wire.setClock(i2c_clock); // I2C clock. Comment this line if having compilation difficulties
  mpu.initialize();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  if (!(mpu_test = mpu.testConnection())) {
    display.clearDisplay();
    display.print("sensor init failed.");
  } else {
    post_init();
    prev = micros();
  }
  return mpu_test;
}

void RboyGame::post_init() {
}

void RboyGame::calibrate(int16_t *offsets) {
  this->calibrate(offsets[0], offsets[1], offsets[2], offsets[3], offsets[4], offsets[5]);
}

void RboyGame::calibrate(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz) {
  mpu.setXAccelOffset(ax);
  mpu.setYAccelOffset(ay);
  mpu.setZAccelOffset(az);
  mpu.setXGyroOffset(gx);
  mpu.setYGyroOffset(gy);
  mpu.setZGyroOffset(gz);
}

void RboyGame::loop() {
  if (!mpu_test) { return; }
  int16_t raw[6];
  uint8_t indice[3];
  for (uint8_t i = 0; i < sizeof(indice); i++) {
    indice[i] = INDEX(coordination_direction, i);
  }
  mpu.getMotion6(raw + indice[0], raw + indice[1], raw + indice[2], raw + 3 + indice[0], raw + 3 + indice[1], raw + 3 + indice[2]);
  for (uint8_t i = 0; i < sizeof(indice); i++) {
    if (NEG(coordination_direction, i)) {
      raw[indice[i]] = -raw[indice[i]];
      raw[3 + indice[i]] = - raw[3 + indice[i]];
    }
  }
  unsigned long time = micros();
  unsigned long dt = time - prev;
  prev = time;
  this->loop(raw, 6, dt);
}

void RboyGame::loop(int16_t *raw, size_t len, unsigned long interval) {
  this->loop(raw[0], raw[1], raw[2], raw[3], raw[4], raw[5], interval);
}

void RboyGame::loop(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, unsigned long interval) {
  if (need_to_redraw) {
    display.clearDisplay();
    display.setCursor(0, 0);
  }
  long remain = frame_interval - interval;
  if (remain > 0) {
    delayMicroseconds(remain);
    interval = frame_interval;
  }
  this->draw_frame(ax, ay, az, gx, gy, gz, interval);
  display.display();
}

void RboyGame::draw_frame(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, unsigned long interval) {
}

void RboyGame::attach_button(int pin) {
  pinMode(pin, INPUT_PULLUP);
  RboyGame::attached_game = this;
  attachInterrupt(digitalPinToInterrupt(pin), RboyGame::button_handler, handle_button_mode());
}

int RboyGame::handle_button_mode() {
  return RISING;
}

void RboyGame::handle_button() {}

void RboyGame::button_handler() {
  if (RboyGame::attached_game) {
    static unsigned long last_interrupted = 0;
    unsigned long curr_interrupted = millis();
    if (curr_interrupted - last_interrupted > 200) {
      RboyGame::attached_game->handle_button();
    }
    last_interrupted = curr_interrupted;
  }
}
