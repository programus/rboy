#include "RboyGame.h"

#include <Wire.h>

RboyGame* RboyGame::attached_game = NULL;

bool RboyGame::initialize() {
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.display();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  mpu.initialize();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  if (!(mpu_test = mpu.testConnection())) {
    display.clearDisplay();
    display.print("sensor init failed.");
  }
  return mpu_test;
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
  mpu.getMotion6(raw, raw + 1, raw + 2, raw + 3, raw + 4, raw + 5);
  unsigned long time = millis();
  unsigned long dt = time - prev;
  prev = time;
  this->draw_frame(raw, 6, dt);
}

void RboyGame::draw_frame(int16_t *raw, size_t len, unsigned long interval) {
  this->draw_frame(raw[0], raw[1], raw[2], raw[3], raw[4], raw[5], interval);
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
    RboyGame::attached_game->handle_button();
  }
}
