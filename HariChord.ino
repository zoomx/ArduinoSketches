/*********************************************************************

Hary Chord for OLED 128x64
by hwiguna
https://github.com/hwiguna
Taken from
https://raw.githubusercontent.com/hwiguna/g33k/master/ArduinoProjects/2015/128x64_OLED/_128x64_OLED/_128x64_OLED.ino
Video
https://www.youtube.com/watch?v=_t_lfcGV_YM

Changed for I2C OLED Module by Zoomx
22/06/2015

Adafruit code license
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x64 size display using SPI to communicate
4 or 5 pins are required to interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//I2C
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void HariChord(int rad, float rot)
{
  byte n = 7;
  int x[n];
  int y[n];
  for (byte i=0; i<n; i++)
  {
    float a = rot + i * 2 * PI / n;
    x[i] = 64 + cos(a)*rad;
    y[i] = 32 + sin(a)*rad;
  }
  
  display.clearDisplay();
  for (byte i=0; i<(n-1); i++)
  {
    for (byte j=i+1; j<n; j++)
    {
      display.drawLine(x[i], y[i], x[j],y[j], WHITE);
    }
  }
  display.display();
}

void setup()   {                
  Serial.begin(9600);
  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(1000);
  
  // Clear the buffer.
  
}


void loop() {
  int nFrames = 36;
  for (float f=0; f < nFrames; f++)
  {
    int rad = f * 64 / nFrames;
    float rot = f * 2*PI / nFrames;
    HariChord( rad, rot );
  }

  for (float f=0; f < nFrames; f++)
  {
    int rad = 64 - (f * 64 / nFrames);
    float rot = 2*PI - (f * 2*PI / nFrames);
    HariChord( rad, rot );
  }
}
