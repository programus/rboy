#include <Arduino.h>
#include "BuzzGame.h"
#include "pins.h"

RboyGame* pGame = NULL;

void setup() {
  pGame = new CountGame();
  pGame->set_coordiation_direction(CD_DXLYBZ);
  pGame->attach_tone(TONE_PIN);
  pGame->attach_button(BUTTON_PIN);
  pGame->initialize();
}

void loop() {
  pGame->loop();
}
