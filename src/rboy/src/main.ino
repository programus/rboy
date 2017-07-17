
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <I2Cdev.h>
#include <MPU6050.h>

#include <math.h>

#define OLED_RESET (-1)
Adafruit_SSD1306 display(OLED_RESET);
MPU6050 mpu;

int16_t raw[6];

unsigned long prev = 0;

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}

void displayLog(const char* message, bool complete=true, uint8_t size=1) {
  display.clearDisplay();
  display.setTextSize(size);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(message);
  if (complete) {
    display.display();
  }
}

void setup() {
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  displayLog("inited display");

  displayLog("initing MPU...");
  mpu.initialize();

  displayLog("Testing device connections...");
  displayLog(mpu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  // supply your own gyro offsets here, scaled for min sensitivity
  // mpu.setXGyroOffset(220);
  // mpu.setYGyroOffset(76);
  // mpu.setZGyroOffset(-85);
  // mpu.setZAccelOffset(1788); // 1688 factory default for my test chip
}

void loop() {
  mpu.getMotion6(raw, raw + 1, raw + 2, raw + 3, raw + 4, raw + 5);
  displayLog("ax\nay\naz\ngx\ngy\ngz\n", false);
  unsigned long time = millis();
  unsigned long fps = 1000 / (time - prev);
  prev = time;
  display.print("fps: ");
  display.println(fps);
  int rate = 32768 / 56;
  for (int i = 0; i < 6; i++) {
    int x0 = 16 + 56;
    int y0 = i * 8 + 1;
    display.drawRect(16, y0, 112, 6, WHITE);
    int len = raw[i] / rate;
    int x1 = x0 + len;
    display.fillRect(min(x0, x1), y0 + 1, abs(len), 4, WHITE);
  }

  display.display();
}
