#include "CountGame.h"
#include "pins.h"

CountGame game;

void setup() {
  game.initialize();
  game.attach_button(BUTTON_PIN, RISING);
}

void loop() {
  game.loop();
}
