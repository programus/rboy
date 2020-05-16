#include "RboyGame.h"
#include <Wire.h>
#include <Arduino.h>

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

#ifdef NEED_CALIBRATE
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
#endif

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
  this->loop_tones(&dt);
  this->loop(raw, 6, dt);
}

void RboyGame::loop_tones(unsigned long* pDt) {
  uint16_t freq = this->_freqs ? (uint16_t)pgm_read_word(this->_freqs + this->playing_index) : 0;
  if (freq == LOOP_TONE) {
    this->playing_index = 0;
    freq = this->_freqs ? (uint16_t)pgm_read_word(this->_freqs + this->playing_index) : 0;
  }
  if (freq) {
    // play tones
    if (this->_duration_remain > 0) {
      if (Serial) {
        Serial.println(F("playing..."));
      }
      if (freq > 10) {
        this->tone(freq, -1);
      } else {
        this->no_tone();
      }
      this->_duration_remain -= *pDt / 1000;
    } else {
      if (Serial) {
        Serial.println(F("next..."));
      }
      this->playing_index++;
      this->_duration_remain = (uint16_t)pgm_read_word(this->_durations + this->playing_index);
    }
  } else {
    this->no_tone();
  }
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

void RboyGame::attach_button(uint8_t pin) {
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

void RboyGame::attach_tone(uint8_t pin) {
  tone_pin = pin;
  pinMode(tone_pin, OUTPUT);
}

void RboyGame::tone(unsigned int frequency, unsigned long duration) {
  if (tone_pin >= 0) {
    ::tone(tone_pin, frequency, duration);
  }
}

void RboyGame::no_tone() {
  if (tone_pin >= 0) {
    ::noTone(tone_pin);
  }
}

void RboyGame::play_tones(const uint16_t* freqs, const uint16_t* durations, bool block) {
  if (tone_pin >= 0) {
    if (block) {
      for (unsigned int i = 0; freqs[i]; i++) {
        this->tone(freqs[i], durations[i]);
        delay(durations[i]);
      }
    } else {
      this->stop_playing();
      this->_freqs = (uint16_t*)freqs;
      this->_durations = (uint16_t*)durations;
      this->playing_index = 0;
      this->_duration_remain = (uint16_t)pgm_read_word(durations);
    }
  }
}

bool RboyGame::is_playing() {
  return this->_freqs;
}

void RboyGame::stop_playing() {
  this->no_tone();
  this->_freqs = NULL;
  this->_durations = NULL;
  this->playing_index = 0;
  this->_duration_remain = 0;
}