/*********************************************************************

Space-Invader-Arduino
=====================

By Costescu Constantin Eugen

The game space invaders for an arduino connected to a nokia 5110 lcd.

authored on 13 Aug 2013
https://github.com/ccostin93/Space-Invader-Arduino
Download the library: https://github.com/ccostin93/Space-Invaders-library

Metro library
http://playground.arduino.cc/code/metro
https://github.com/thomasfredericks/Metro-Arduino-Wiring

2015 May
All libraries included here by zoomx
Changed name from Space_Invaders to Space_Invaders.ino
but get error of main.cpp undefined reference to `setup' & `loop' because of Space_Invaders.cpp
so renamed another time to Space_Invaders_Nokia.ino
changed static unsigned char PROGMEM to const unsigned char PROGMEM
changed Metro(300,1) to Metro(300) since Metro get only one parameter, maybe an older version?

added #include <SPI.h>
Changes Buttons, Nokia and speaker pins for Funduino JoyStick Shield

Funduino Joystick Shield buttons
int up_button = 2;
int down_button = 4; not used here
int left_button = 5; not used here
int right_button = 3; not used here
int start_button = 6; not used here
int select_button = 7; not used here
int analog_button = 8; joystick button
int x_axis = A0;
int y_axis = A1; not used here

Funduino Joystick Shield Nokia pins
pin 13 - Serial clock out (SCLK)
pin 12 - Serial data out (DIN)
pin 11 - Data/Command select (D/C)
pin 10 - LCD chip select (CS)
pin 9  - LCD reset (RST)

Added define for Joystick pin and for Joystick max value because is not 1023
Added define for contrast
***********************************************************************/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "Metro.h"
#include "Space_Invaders.h"

const unsigned char PROGMEM over[] =
{  
  B01100010,
  B10010100,
  B10011000,
  B10010000,
  B11111110,
  B00000000,
  B10000010,
  B10010010,
  B10010010,
  B10010010,
  B11111110,
  B00000000,
  B11111000,
  B00000100,
  B00000010,
  B00000100,
  B11111000,
  B00000000,
  B01111100,
  B10000010,
  B10000010,
  B10000010,
  B01111100 };

const unsigned char PROGMEM game[] =
{   
  B10000010,
  B10010010, 
  B10010010,
  B10010010,
  B11111110, 
  B00000000, 
  B11111110,
  B01000000,
  B00111000,
  B01000000, 
  B11111110,
  B00000000, 
  B00111110, 
  B01001000, 
  B10001000,
  B01001000,
  B00111110, 
  B00000000, 
  B01001110, 
  B10001010, 
  B10000010, 
  B10000010,
  B01111100 };

const unsigned char PROGMEM pause[] = {
  B00000000,
  B00000000,
  B10000010,
  B10010010,
  B10010010,
  B10010010,
  B11111110,
  B00000000,
  B01001100,
  B10010010,
  B10010010,
  B10010010,
  B01100100,
  B00000000,
  B11111100,
  B00000010,
  B00000010,
  B00000010,
  B11111100,
  B00000000,
  B00111110,
  B01001000,
  B10001000,
  B01001000,
  B00111110,
  B00000000,
  B01100000,
  B10010000,
  B10010000,
  B10010000,
  B11111110,
  B00000000
};

Adafruit_PCD8544 display = Adafruit_PCD8544(13, 12, 11, 10, 9);     //was(7, 6, 5, 4, 3);

#define CONTRAST 60

#define JOY_ANALOG_PIN 0
#define JOY_MAX_VALUE 675
#define JPIN 8 //joystick button pin
#define BPIN 2 //button pin
#define counter 1000 //debounce time

//Shift Register - aprinde cifra
#define data 9
#define stcp 10 
#define shcp 11

//Shift Register - schimba ce cifra apare dintre cele 4
#define dataB 12
#define CP 13

#define nr_enemy 12
#define chance_shoot 4

unsigned int joyX; //joystick x value

char bounce; //the enemy bounces off the wall
char play = 0; // 0 - play, 1 - pause, 2 - defeat 

//byte chance_shoot = 4;
Enemy *enemy[nr_enemy];
Bullet *ShipBullet = new Bullet();
Bullet *EnemyBullet = new Bullet();
Ship *ship;

byte level = 1;
unsigned int score = 0;

/*
Doesn't compile Metro now expect only one parameter.
Metro s = Metro(300,1), b = Metro(150,1), e = Metro(1000,1);
Metro led = Metro(30, 1);
*/

Metro s = Metro(300), b = Metro(150), e = Metro(1000);
Metro led = Metro(30);


char oldB = 0;
char oldJB = 1;

void setup()  {
  Serial.begin(9600);
  Serial.println("Space_Invaders");
  //Buttons
  pinMode(BPIN, INPUT);
  pinMode(JPIN, INPUT);
  //Shift Register
  pinMode(data,OUTPUT);
  pinMode(stcp,OUTPUT);
  pinMode(shcp,OUTPUT);
  pinMode(dataB, OUTPUT);
  pinMode(CP, OUTPUT);

  display.begin();
  display.setContrast(CONTRAST);
  initGame(level);
}

void loop() {
  if (play == 0) { //play
    if(e.check() == 1) {
      moveEnemy();
    }
    if(s.check() == 1) {
      buttonsPressed(); //check to see if any buttons have been pressed
      moveShip();
      if (ship->getLives() == 0) {
        play = 2;
      }
    }
    if(b.check()) {
      moveBullet();
      if (remaining_enemys() == 0) {
        level += 1;
        initGame(level);
      }
    }
  }
  else if (play == 1) { //pause
    buttonsPressed();
    showPause();
  }
  else { //play == 2 - defeat/dead
    showMenu();
    displayScore(score);
    buttonsPressed();
  }
  
}

void addBullet(byte x, byte y, byte newSpeed, char side) {
  //side == 1 -> my ship, else enemy ship
  if (side == 1 && ShipBullet->getExist() == 0) {
    ShipBullet = new Bullet(x, y, newSpeed);
  }
  else if (side == 0 && EnemyBullet->getExist() == 0) {
    EnemyBullet = new Bullet(x, y, newSpeed);
  }
}

char getButton(char button, char oldB) {
  static short count[14] = {
    0,                                                                                                                                                         };
  static char prevB[14] = {
    0,                                                                                                                                                         };
  char b = digitalRead(button);
  if (b == prevB[button]) {
    count[button]++;
  }
  else {
    count[button] = 0;
  }
  prevB[button] = b;
  if (count[button] == counter) {
    count[button] = 0;
    return b;
  }
  else return prevB[button];
}




