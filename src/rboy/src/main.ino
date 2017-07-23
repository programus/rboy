#include "games/CountGame.h"
#include "games/RawDataGame.h"
#include "games/SnakeGame.h"
#include "pins.h"

RboyGame* pGame = NULL;

void setup() {
  randomSeed(analogRead(0));
  // pGame = new RawDataGame();
  uint16_t selection = random(2);
  switch (selection) {
  case 0:
    pGame = new CountGame();
    break;
  case 1:
    pGame = new SnakeGame();
    break;
  }
  pGame->set_coordiation_direction(CD_DXLYBZ);
  pGame->attach_button(BUTTON_PIN);
  pGame->initialize();
}

void loop() {
  pGame->loop();
}
