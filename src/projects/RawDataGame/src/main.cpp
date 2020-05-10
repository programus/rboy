#include <Arduino.h>
#include "RawDataGame.h"
#include "pins.h"

RboyGame* pGame = NULL;

void setup() {
  Serial.begin(9600);
  pGame = new RawDataGame();
  pGame->set_coordiation_direction(CD_DXLYBZ);
  pGame->attach_button(BUTTON_PIN);
  pGame->attach_tone(TONE_PIN);
  pGame->initialize();
}

void loop() {
  pGame->loop();
}
