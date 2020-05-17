#include <Arduino.h>
#include "FoodieGame.h"
#include "pins.h"

RboyGame* pGame = NULL;

void setup() {
  Serial.begin(250000);
  pGame = new FoodieGame();
  pGame->set_coordiation_direction(CD_DXLYBZ);
  pGame->attach_button(BUTTON_PIN);
  pGame->attach_tone(TONE_PIN);
  pGame->initialize();
}

void loop() {
  pGame->loop();
}
