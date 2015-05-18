/**
* Pin 2 & 3 = Player 1 buttons down & up
* Pin 5 & 6 = Player 2 buttons down & up
**/
#include "Controller.h"
#include "Arduino.h"

Controller::Controller() { } 

Controller::Controller(const byte p) { 
  Controller::player = p;
  if (Controller::player == Controller::PLAYER_1) { 
    Controller::playername = "PLAYER1";
  } else { 
    Controller::playername = "PLAYER2";
  }
}

void activate(const byte p) { 
  new Controller(p);
}

byte Controller::getPressed() {
  if (Controller::player == Controller::PLAYER_1) { 
    int state2 = digitalRead(10);
    int state3 = digitalRead(5);
    if (state2 == HIGH) { 
      return Controller::UP;
    } else if (state3 == HIGH) { 
      return Controller::DOWN;
    }
  } else if (Controller::player == Controller::PLAYER_2) { 
    int state5 = digitalRead(6);
    int state6 = digitalRead(7);
    if (state5 == HIGH) {
      return Controller::UP;
    } else if (state6 == HIGH) { 
      return Controller::DOWN;
    } 
  }
  return Controller::NOTHING;
}

boolean Controller::hasBeenPressed() {
  int state2 = digitalRead(10);
  int state3 = digitalRead(5);
  int state5 = digitalRead(6);
  int state6 = digitalRead(7);  
  if (state2 == HIGH || state3 == HIGH || state5 == HIGH || state6 == HIGH) { 
    return true;
  }
  return false;
}

void Controller::setName(String newName) { 
  Controller::playername = newName;
}

String Controller::getName() { 
  return Controller::playername;
}

/*boolean Controller::middlePressed() { 
  int state4 = digitalRead(4);
  if (state4 == HIGH) { 
    return true;
  }
  return false;
}*/

