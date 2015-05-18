#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "Arduino.h"

class Controller {
  
  public:  
    Controller();
    Controller(const byte p);
    void start(const byte p);
    static const byte PLAYER_1 = 0;
    static const byte PLAYER_2 = 1;
    static const byte UP = 2;
    static const byte DOWN = 3;
    static const byte MIDDLE = 5;
    static const byte NOTHING = 6;
    byte getInput();
    void setName(String newName);
    String getName();
//    static boolean middlePressed();
    byte getPressed();
    static boolean hasBeenPressed();
  private:
    String playername;
    byte player;
};

#endif

