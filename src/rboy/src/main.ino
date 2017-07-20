#include "games/CountGame.h"
#include "games/RawDataGame.h"
#include "pins.h"

RboyGame* pGame = NULL;

void setup() {
  pGame = new CountGame();
  // pGame = new RawDataGame();
  pGame->initialize();
  pGame->attach_button(BUTTON_PIN);
}

void loop() {
  pGame->loop();
}
