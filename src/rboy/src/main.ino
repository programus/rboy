
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <I2Cdev.h>
#include <MPU6050.h>

#include <math.h>

#define OLED_RESET  (-1)
#define BUTTON_PIN  2
#define MAX_DISP_NUM     100
#define LCDW        SSD1306_LCDWIDTH
#define LCDH        SSD1306_LCDHEIGHT
#define FONTW       6
#define FONTH       8

#define LIMIT       3000L

const long MAX_NUM = MAX_DISP_NUM * LIMIT;

Adafruit_SSD1306 display(OLED_RESET);
MPU6050 mpu;
bool mpu_test;

unsigned long prev = 0;

volatile long number = 0;

void display_log(const char* message, bool complete=true, uint8_t size=1) {
  display.clearDisplay();
  display.setTextSize(size);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(message);
  if (complete) {
    display.display();
  }
}

void show_number(int n, uint8_t size=5) {
  display.clearDisplay();
  display.setTextSize(size);
  display.setTextColor(WHITE);
  uint16_t fw = FONTW * size;
  uint16_t fh = FONTH * size;
  uint16_t x = (LCDW >> 1) - fw;
  uint16_t y = (LCDH - fh) >> 1;
  uint16_t mx = 4;
  uint16_t my = 2;
  for (int i = 0; i < 2; i++) {
    int offset = i << 1;
    display.drawRect(x - mx - 1 - offset, y - my - 1 - offset, (fw << 1) + (mx << 1) + (offset << 1), fh + (my << 1) + (offset << 1), WHITE);
  }
  if (n < 10) {
    x += fw;
  }
  display.setCursor(x, y);
  display.print(n);
}

void button_pressed() {
  number = LIMIT >> 1;
}

void setup_button() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), button_pressed, RISING);
}

void setup_i2c_devices() {
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  show_number(number);
  mpu.initialize();
  if (!(mpu_test = mpu.testConnection())) {
    display_log("MPU6050 connection failed");
  }
}

void setup_mpu_offset() {
  // supply your own gyro offsets here, scaled for min sensitivity
  // mpu.setXGyroOffset(220);
  // mpu.setYGyroOffset(76);
  // mpu.setZGyroOffset(-85);
  // mpu.setZAccelOffset(1788); // 1688 factory default for my test chip
}

void setup() {
  setup_button();
  setup_i2c_devices();
  setup_mpu_offset();
}

void loop() {
  if (!mpu_test) { return; }
  int16_t raw[6];
  mpu.getMotion6(raw, raw + 1, raw + 2, raw + 3, raw + 4, raw + 5);
  double ayz = sqrt(sq((unsigned long)raw[1]) + sq((unsigned long)raw[2]));
  double sinv = max(-1.0, min(1.0, ayz / ((double) 0x3fff)));
  double value = (M_PI_4 - abs(asin(sinv))) * 10;

  long delta = (long) value;
  unsigned long time = millis();
  long dt = time - prev;
  prev = time;
  number += delta * dt;
  if (number <= 0) {
    number = MAX_NUM - 1;
  }
  number = number % MAX_NUM;
  display.invertDisplay(delta < 0);
  show_number(number / LIMIT);

  // display.setCursor(0, 0);
  // display.setTextSize(1);

  display.display();
}
