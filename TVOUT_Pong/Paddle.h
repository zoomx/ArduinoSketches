#ifndef PADDLE_H
#define PADDLE_H
#include "Arduino.h"
#include "Controller.h"

class Paddle { 
  
  public:
    Paddle();
    Paddle(boolean player_one, char ax, char ay);
    void show();
  private:
    Controller controller;
    boolean P1;
    char x;
    char y;
};

#endif

