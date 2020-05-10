#include <Arduino.h>
#include "SnakeGame.h"
#include "pins.h"

RboyGame* pGame = NULL;

void setup() {
  Serial.begin(9600);
  pGame = new SnakeGame();
  pGame->set_coordiation_direction(CD_DXLYBZ);
  pGame->attach_button(BUTTON_PIN);
  pGame->attach_tone(TONE_PIN);
  pGame->initialize();
}

void loop() {
  pGame->loop();
}
