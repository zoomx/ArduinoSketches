/*********************************************************


*********************************
* Snake Game by Abhinav Faujdar *
*********************************


https://www.youtube.com/watch?v=JUSWurw_I_o

Some changes for 1.6.4 IDE by zoomx May 2015
added #include <SPI.h>
Changes Buttons, Nokia and speaker pins for Funduino JoyStick Shield

Funduino Joystick Shield buttons
int up_button = 2;
int down_button = 4;
int left_button = 5;
int right_button = 3;
int start_button = 6; not used here
int select_button = 7; not used here
int analog_button = 8;
int x_axis = A0;
int y_axis = A1;

Funduino Joystick Shield Nokia pins
pin 13 - Serial clock out (SCLK)
pin 12 - Serial data out (DIN)
pin 11 - Data/Command select (D/C)
pin 10 - LCD chip select (CS)
pin 9  - LCD reset (RST)

library and diagram: :http://www.sendspace.com/file/2hfu5k
not available anymore in May 2015

****************************************************************/

#include <SPI.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>


Adafruit_PCD8544 display = Adafruit_PCD8544(13, 12, 11, 10, 9);  //Initialise display object
// was Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

#define CONTRAST 70

/******* constants *****/
#define LEFT 5    //was 8
#define DOWN 4    //was 10
#define RIGHT 3   //was 11
#define UP 2      // was 12
#define PAUSE 8   //was 9
#define MAX_WIDTH 84        //display 84x48
#define MAX_HEIGHT 48
#define speakerPin 6

boolean dl = false, dr = false, du = false, dd = false; // to check in which direction the snake is currently moving

int x[200], y[200], i, slength, tempx = 10, tempy = 10, xx, yy;
unsigned int high;
uint8_t bh, bl;
int xegg, yegg;
int freq, tb;
int l, r, u, d, p;
unsigned long time = 280, beeptime = 50;
int score = 0, flag = 0;



void setup()
{
  Serial.begin(9600);         //Begin Serial Communication
  Serial.println("Nokia Snake by Abhinav Faujdar");
  display.begin();
  display.clearDisplay();



  pinMode(LEFT, INPUT);        //Directional keys and pause
  pinMode(RIGHT, INPUT);
  pinMode(UP, INPUT);
  pinMode(DOWN, INPUT);
  pinMode(PAUSE, INPUT);

  pinMode(speakerPin, OUTPUT); //Buzzer pin

  digitalWrite(LEFT, HIGH);    //Active low keys
  digitalWrite(RIGHT, HIGH);
  digitalWrite(UP, HIGH);
  digitalWrite(DOWN, HIGH);
  digitalWrite(PAUSE, HIGH);

  display.setContrast(CONTRAST);
  slength = 8;               //Start with snake length 8

  xegg = (display.width()) / 2;
  yegg = (display.height()) / 2;

  display.setTextSize(2);          //Initial Display
  display.setTextColor(BLACK);
  display.setCursor(10, 15);
  display.print(F("SNAKE"));
  display.display();
  delay(4000);
  display.clearDisplay();

  for (i = 0; i <= slength; i++) //Set starting coordinates of snake
  {
    x[i] = 25 - 3 * i;
    y[i] = 10;
  }

  for (i = 0; i < slength; i++)  //Draw the snake
  {
    display.drawCircle(x[i], y[i], 1, BLACK);
  }
  display.display();

  dr = true;  //Going to move right initially
}


//Movement Begins after here
void loop()
{
  movesnake();    //This is called endlessly
}

void movesnake()
{
  l = digitalRead(LEFT);    //Continuously Check the Keys
  d = digitalRead(DOWN);
  r = digitalRead(RIGHT);
  u = digitalRead(UP);
  p = digitalRead(PAUSE);

  if (flag == 0)
  {
    direct();    //When key is pressed,this will change the coordinates accordingly and set flag to 1
    //flag would be set to 1 so that direction is not changed multiple times in a short duration
  }


  if (millis() % time == 0) //this condition becomes true after every 'time' milliseconds...millis() returns the time since launch of program
  {

    if (flag == 0)                                //flag 0 means no directional key has been pressed in the last 'time' milliseconds
    {
      if (dr == true) {
        tempx = x[0] + 3;  // so the snake moves one step in the direction it is moving currently
        tempy = y[0];
      }
      if (dl == true) {
        tempx = x[0] - 3;  //The new coordinates of head of snake goes in tempx,tempy
        tempy = y[0];
      }
      if (du == true) {
        tempy = y[0] - 3;
        tempx = x[0];
      }
      if (dd == true) {
        tempy = y[0] + 3;
        tempx = x[0];
      }
    }

    flag = 0;
    checkgame();                              //Check if snake has met egg or coincided with itself
    checkegg();

    if (tempx <= 0) {
      tempx = 84 + tempx; //If the new coordinates are out of screen, set them accordingly
    }
    if (tempx >= 84) {
      tempx = tempx - 84;
    }
    if (tempy <= 0) {
      tempy = 48 + tempy;
    }
    if (tempy >= 48) {
      tempy = tempy - 48;
    }

    for (i = 0; i <= slength; i++) //Change the coordinates of all points of snake
    {
      xx = x[i];
      yy = y[i];
      x[i] = tempx;
      y[i] = tempy;
      tempx = xx;
      tempy = yy;
    }

    drawsnake();           //Draw the snake and egg at the new coordinates
  }
}


void checkgame()       //Game over checker
{
  for (i = 1; i < slength; i++)        //Checking if the coordinates of head have become equal to one of the non head points of snake
  {
    if (x[i] == x[0] && y[i] == y[0])
    {
      bh = EEPROM.read(1);
      bl = EEPROM.read(0);
      high = (((0xff00 + bh) << 8) + bl);

      if (score > high)
      {
        high = score;
        bh = (high >> 8);
        bl = high & 0xff;

        EEPROM.write(1, bh);
        EEPROM.write(0, bl);
      }

      display.clearDisplay();
      display.setTextColor(BLACK);
      display.setTextSize(1);
      display.setCursor(20, 12);
      display.print("Game Over");
      display.setCursor(15, 30);
      display.print("Score: ");
      display.print(score);
      display.setCursor(15, 40);
      display.print("High: ");
      display.print(high);

      display.display();
      beep(20, 5000);

      display.clearDisplay();

      slength = 8;            //Resetting the values
      score = 0;
      time = 280;

      redraw();              //Restart game by drawing snake with the resetted length and score
    }
  }
}

void checkegg()      //Snake meets egg
{
  if (x[0] == xegg or x[0] == (xegg + 1) or x[0] == (xegg + 2) or x[0] == (xegg - 1)) //Snake in close vicinity of egg
  {
    if (y[0] == yegg or y[0] == (yegg + 1) or y[0] == (yegg + 2) or y[0] == (yegg - 1))
    {
      score += 1;                     //Increase length,score and increase movement speed by decreasing 'time'
      slength += 1;
      if (time >= 90)
      {
        time -= 20;
      }
      display.fillRect(xegg, yegg, 3, 3, WHITE);  //Delete the consumed egg
      display.display();
      beep(35, beeptime);             //Beep with a sound of 35Hz for 'beeptime' ms
      xegg = random(1, 80);           //Create New egg randomly
      yegg = random(1, 40);
    }
  }
}


void direct()                  //Check if user pressed any keys and change direction if so
{
  if (l == LOW and dr == false)  //when key LEFT is pressed ,L will become low
  {
    dl = true; du = false; dd = false;
    tempx = x[0] - 3;            //Save the new coordinates of head in tempx,tempy
    tempy = y[0];
    flag = 1;                   //Do not change direction any further for the ongoing 'time' milliseconds
  }
  else if (r == LOW and dl == false)
  {
    dr = true; du = false; dd = false;
    tempx = x[0] + 3;
    tempy = y[0];
    flag = 1;
  }
  else if (u == LOW and dd == false)
  {
    du = true; dl = false; dr = false;
    tempy = y[0] - 3;
    tempx = x[0];
    flag = 1;
  }
  else if (d == LOW and du == false)
  {
    dd = true; dl = false; dr = false;
    tempy = y[0] + 3;
    tempx = x[0];
    flag = 1;
  }
  else if (p == LOW)            //Pause game for 5 seconds
  {
    display.clearDisplay();
    display.setTextColor(BLACK);
    for (i = 5; i > 0; i--)
    {
      display.setCursor(25, 10);
      display.setTextSize(1);
      display.print(F("PAUSED"));
      display.setCursor(40, 30);
      display.print(i);
      display.display();
      delay(1000);
      display.clearDisplay();
    }
    redraw();          //Redraw the snake and egg at the same position as it was
  }
}


void drawsnake()        //Draw snake and egg at newly changed positions
{
  display.fillRect(xegg, yegg, 3, 3, BLACK);   //Draw egg at new pos
  display.drawCircle(x[0], y[0], 1, BLACK);    //Draw new head of snake
  display.drawCircle(x[slength], y[slength], 1, WHITE); //Delete old tail of snake
  display.display();
}

void redraw()   //Redraw ALL POINTS of snake and egg
{
  display.fillRect(xegg, yegg, 3, 3, BLACK);
  for (i = 0; i < slength; i++)
  {
    display.drawCircle(x[i], y[i], 1, BLACK);
  }
  display.display();
}



void beep (int freq, long tb)          //This function creates a sound of frequency 'freq' Hz and for a duration of 'tb' milliseconds
{
  int x;
  long delayAmount = (long)(500 / freq);     //Calculate time duration of half cycle
  long loopTime = (long)(tb / (delayAmount * 2)); //Calculate no. of cycles
  for (x = 0; x < loopTime; x++) 	 // One iteration produces sound for one cycle
  {
    digitalWrite(speakerPin, HIGH);  //High for half cycle
    delay(delayAmount);
    digitalWrite(speakerPin, LOW);   //Low for half cycle
    delay(delayAmount);
  }
  delay(2);
  //a little delay to make all notes sound separate
}
