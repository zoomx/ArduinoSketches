/*********************************************************************************************
ArduinoTetris

Nokia 5110 implementation and
Funduino Joystick Shield adapting by zoomx

Doesn't work properly
too much flicker...
Buttons seems not work
this port is not good!

Playtune library updater for IDE 1.6.4

Funduino Joystick Shield Nokia pins
pin 13 - Serial clock out (SCLK)
pin 12 - Serial data out (DIN)
pin 11 - Data/Command select (D/C)
pin 10 - LCD chip select (CS)
pin 9  - LCD reset (RST)

Funduino Joystick Shield buttons
int up_button = 2; rotate
int down_button = 4; not used here
int left_button = 5;
int right_button = 3;
int start_button = 6; not used here
int select_button = 7; not used here
int analog_button = 8; not used here
int x_axis = A0; not used here
int y_axis = A1; not used here

In PROGMEM changed from static to const
Topic: Issue with PROGMEM data, broken code?
http://forum.arduino.cc/index.php?topic=303156.0

Changed from btn_analog to btn_Digital in AdvButton 

Original MicroView Arduino-compatible C port by Richard Birkby https://github.com/rbirkby/ArduinoTetris
Original JavaScript implementation - Jake Gordon - https://github.com/jakesgordon/javascript-tetris
MIT licenced
*********************************************************************************************/

#define ROTATEBTN 2
#define LEFTBTN 5
#define RIGHTBTN 3

#define CONTRAST 35

#include <avr/pgmspace.h>
//#include <MicroView.h>
#include <Adafruit_GFX.h>   //If I put this under #include <Adafruit_PCD8544.h> I get error of Adafruit_GFX.h not found!
#include <Adafruit_PCD8544.h>
#include <AdvButton.h>		//http://playground.arduino.cc/Code/AdvButton
#include <ButtonManager.h>
#include "Tetris.h"
#include "Playtune.h"

#include <SPI.h>

// Software SPI (slower updates, more flexible pin options):
Adafruit_PCD8544 display = Adafruit_PCD8544(13, 12, 11, 10, 9);

Playtune pt;

//-------------------------------------------------------------------------
// game constants
//-------------------------------------------------------------------------

enum DIR {
  UP,
  RIGHT,
  DOWN,
  LEFT,
  MIN = UP,
  MAX = LEFT
};

const int nx = 10; // width of tetris court (in blocks)
const int ny = 12; // height of tetris court (in blocks)

const int rotateButton = ROTATEBTN;
const int leftButton = LEFTBTN;
const int rightButton = RIGHTBTN;

const int audioPin1 = A3;
const int audioPin2 = A4;

//-------------------------------------------------------------------------
// game variables (initialized during reset)
//-------------------------------------------------------------------------

const int dx = (84 - 24) / nx;   // pixel size of a single tetris block
const int dy = 48 / ny;
int blocks[nx][ny];                               // 2 dimensional array (nx*ny) representing tetris court - either empty block or occupied by a 'piece'
bool playing = true;                              // true|false - game is in progress
bool lost = false;
Piece current, next;                              // the current and next piece
unsigned int score = 0;                           // the current score
int dir = 0;

//-------------------------------------------------------------------------
// tetris pieces
//
// blocks: each element represents a rotation of the piece (0, 90, 180, 270)
//         each element is a 16 bit integer where the 16 bits represent
//         a 4x4 set of blocks, e.g. j.blocks[0] = 0x44C0
//
//             0100 = 0x4 << 3 = 0x4000
//             0100 = 0x4 << 2 = 0x0400
//             1100 = 0xC << 1 = 0x00C0
//             0000 = 0x0 << 0 = 0x0000
//                               ------
//                               0x44C0
//
//-------------------------------------------------------------------------

unsigned int i[] = {0x0F00, 0x2222, 0x00F0, 0x4444};
unsigned int j[] = {0x44C0, 0x8E00, 0x6440, 0x0E20};
unsigned int l[] = {0x4460, 0x0E80, 0xC440, 0x2E00};
unsigned int o[] = {0xCC00, 0xCC00, 0xCC00, 0xCC00};
unsigned int s[] = {0x06C0, 0x8C40, 0x6C00, 0x4620};
unsigned int t[] = {0x0E40, 0x4C40, 0x4E00, 0x4640};
unsigned int z[] = {0x0C60, 0x4C80, 0xC600, 0x2640};

AdvButton but1 = AdvButton(leftButton, onLeftButton, 50, 200, btn_Digital);
AdvButton but2 = AdvButton(rightButton, onRightButton, 50, 200, btn_Digital);
AdvButton but3 = AdvButton(rotateButton, onRotateButton, 100, 200, btn_Digital);

//------------------------------------------------
// do the bit manipulation and iterate through each
// occupied block (x,y) for a given piece
//------------------------------------------------
void getPositionedBlocks(unsigned int type[], int x, int y, int dir, Block* positionedBlocks) {
  unsigned int bit;
  int blockIndex = 0, row = 0, col = 0, blocks = type[dir];

  for (bit = 0x8000; bit > 0 ; bit = bit >> 1) {
    if (blocks & bit) {
      Block block = {x + col, y + row};

      positionedBlocks[blockIndex++] = block;
    }
    if (++col == 4) {
      col = 0;
      ++row;
    }
  }
}

//-----------------------------------------------------
// check if a piece can fit into a position in the grid
//-----------------------------------------------------
bool occupied(unsigned int type[], int x, int y, int dir) {
  bool result = false;

  Block positionedBlocks[4];
  getPositionedBlocks(type, x, y, dir, positionedBlocks);

  for (int i = 0; i < 4; i++) {
    Block block = positionedBlocks[i];

    if ((block.x < 0) || (block.x >= nx) || (block.y < 0) || (block.y >= ny) || getBlock(block.x, block.y))
      result = true;
  }

  return result;
}

bool unoccupied(unsigned int type[], int x, int y, int dir) {
  return !occupied(type, x, y, dir);
}

//-----------------------------------------
//-----------------------------------------
unsigned int *pieces[] = {i, j, l, o, s, t, z};

Piece randomPiece() {
  Piece current = {pieces[random(0, 6)], 3, 0, UP};
  return current;
}

//-------------------------------------------------------------------------
// GAME LOOP
//-------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  Serial.println("ArduinoTetris on Nokia LCD");
  //uView.begin();
  display.begin();
  display.setContrast(CONTRAST);
  //uView.clear(ALL);
  display.clearDisplay();
  //uView.display();
  display.display();

  randomSeed(millis());

  pt.tune_initchan(audioPin1);
  pt.tune_initchan(audioPin2);

  reset();
}

void loop() {
  ButtonManager::instance()->checkButtons();
  int time = millis();

  if (time % 60 == 0) {
    draw();

    if (playing && (time % 300 == 0)) {
      if (!pt.tune_playing) {
        pt.tune_playscore(tetrisScore);
      }

      drop();

      if (lost) {
        draw();
        delay(1000);
      }
    }
  }
}

void onLeftButton(AdvButton* but) {
  if (lost) {
    reset();
  }

  move(LEFT);
}

void onRightButton(AdvButton* but) {
  if (lost) {
    reset();
  }

  move(RIGHT);
}

void onRotateButton(AdvButton* but) {
  if (lost) {
    reset();
  }

  rotate();
}

//-------------------------------------------------------------------------
// GAME LOGIC
//-------------------------------------------------------------------------

void lose() {
  playing = false;
  lost = true;
  pt.tune_stopscore();
}

void addScore(int n) {
  score = score + n;
}
void clearScore() {
  score = 0;
}
int getBlock(int x, int y) {
  return (blocks[x] ? blocks[x][y] : NULL);
}
void setBlock(int x, int y, int type) {
  blocks[x][y] = type;
}
void clearBlocks() {
  for (int x = 0; x < nx; x++) {
    for (int y = 0; y < ny; y++) {
      blocks[x][y] = NULL;
    }
  }
}
void setCurrentPiece(Piece piece) {
  current = piece;
}
void setNextPiece(Piece piece) {
  next = piece;
}

void reset() {
  lost = false;
  playing = true;
  clearBlocks();
  clearScore();
  setCurrentPiece(randomPiece());
  setNextPiece(randomPiece());
}

bool move(int dir) {
  int x = current.x, y = current.y;
  switch (dir) {
    case RIGHT: x = x + 1; break;
    case LEFT:  x = x - 1; break;
    case DOWN:  y = y + 1; break;
  }
  if (unoccupied(current.type, x, y, current.dir)) {
    current.x = x;
    current.y = y;
    return true;
  }
  else {
    return false;
  }
}

void rotate() {
  int newdir = (current.dir == MAX ? MIN : current.dir + 1);
  if (unoccupied(current.type, current.x, current.y, newdir)) {
    current.dir = newdir;
  }
}

void drop() {
  if (!move(DOWN)) {
    addScore(10);
    dropPiece();
    removeLines();
    setCurrentPiece(next);
    setNextPiece(randomPiece());

    if (occupied(current.type, current.x, current.y, current.dir)) {
      lose();
    }
  }
}

void dropPiece() {
  Block positionedBlocks[4];
  getPositionedBlocks(current.type, current.x, current.y, current.dir, positionedBlocks);

  for (int i = 0; i < 4; i++) {
    setBlock(positionedBlocks[i].x, positionedBlocks[i].y, -1);
  }
}

void removeLines() {
  int x, y, n = 0;
  bool complete;

  for (y = ny; y > 0; --y) {
    complete = true;

    for (x = 0; x < nx; ++x) {
      if (!getBlock(x, y)) complete = false;
    }

    if (complete) {
      removeLine(y);
      y = y + 1; // recheck same line
      n++;
    }
  }
  if (n > 0) {
    addScore(100 * pow(2, n - 1)); // 1: 100, 2: 200, 3: 400, 4: 800
  }
}

void removeLine(int n) {
  int x, y;
  for (y = n; y >= 0; --y) {
    for (x = 0; x < nx; ++x) {
      setBlock(x, y, (y == 0) ? NULL : getBlock(x, y - 1));
    }
  }
}

//-------------------------------------------------------------------------
// RENDERING
//-------------------------------------------------------------------------

void draw() {
  drawCourt();
  drawNext();
  drawScore();

  removeLines();
  //uView.display();
  display.display();
}

void drawCourt() {
  //uView.clear(PAGE);
  display.clearDisplay();
  if (playing)
    drawPiece(current.type, current.x + 6, current.y, current.dir);

  int x, y;
  for (y = 0 ; y < ny ; y++) {
    for (x = 0 ; x < nx ; x++) {
      if (getBlock(x, y))
        drawBlock(x + 6, y);
    }
  }

  if (lost) {
    //uView.setCursor(0, 20);
    //uView.print(" Game Over ");
    display.setCursor(0, 20);
    display.print(" Game Over ");
    display.display();
  }
}

void drawNext() {
  if (playing)
    drawPiece(next.type, 0, 3, next.dir);
}

void drawScore() {
  //uView.setCursor(0, 0);
  display.setCursor(0, 0);
  //uView.print(score);
  display.print(score);
  display.display();
}

void drawPiece(unsigned int type[], int x, int y, int dir) {
  Block positionedBlocks[4];
  getPositionedBlocks(type, x, y, dir, positionedBlocks);

  for (int i = 0; i < 4; i++) {
    drawBlock(positionedBlocks[i].x, positionedBlocks[i].y);
  }
}

void drawBlock(int x, int y) {
  //uView.rect(x*dx, y*dy, dx, dy);
  display.drawRect(x * dx, y * dy, dx, dy, BLACK);
  display.display();
}
