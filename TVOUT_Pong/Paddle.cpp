#include "Paddle.h"
#include "Arduino.h"
#include "Controller.h"

Paddle::Paddle() { }

Paddle::Paddle(boolean player_one, char ax, char ay) { 
  Paddle::P1 = player_one;
  Paddle::x = ax;
  Paddle::y = ay;
  if (Paddle::P1) { 
    Paddle::controller.start(Controller::PLAYER_1);
  } else { 
    Paddle::controller.start(Controller::PLAYER_2);
  }
}

void Paddle::show() {
  
}

