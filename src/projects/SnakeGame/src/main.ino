#include "SnakeGame.h"
#include "pins.h"

RboyGame* pGame = NULL;

void setup() {
  pGame = new SnakeGame();
  pGame->set_coordiation_direction(CD_DXLYBZ);
  pGame->attach_button(BUTTON_PIN);
  pGame->initialize();
}

void loop() {
  pGame->loop();
}