/*
Stepper05SerialControl2
 Added serial control
 two Stepper Motor Control
 Move the motor depending on commands received
 Using two EasyDriver stepper module
 
 Need optimisation to remove doubled finctions
 
 2015 02 25 by zoomx
 */

#define LEDPIN 13  //Led is on pin 13 in Arduino UNO boards.

#define STEP1PULSE 9
#define STEP1DIR 8
#define STEP1HALF 7
#define STEP2PULSE 6
#define STEP2DIR 5
#define STEP2HALF 4

#define DELAY 2000

//Serial input with termination section
#define INLENGTH 5          //Max string lenght over serial. Needed for input with termination
#define INTERMINATOR 13     //GetCharFromSerial routine cycle waiting for this character. Needed for input with termination
char inString[INLENGTH+1];  //GetCharFromSerial returns this char array. Needed for input with termination
char comm;  //First character received. Needed for input with termination

int Distance = 0;  // Record the number of steps we've taken

void PrintVersion(){
  Serial.println("Stepper05SerialControl2");
  Serial.print(F("Version 0.1"));
  Serial.print(__DATE__);  //this is the compiling date
  Serial.print(F(" "));
  Serial.println(__TIME__); //this is the compiling time
  Serial.println();
}

void GetCharFromSerial(){
  //Get string from serial and put in inString
  //first letter in comm
  //http://forums.adafruit.com/viewtopic.php?f=8&t=9918

  Serial.flush(); //flush all previous received and transmitted data
  byte inCount = 0;
  do
  {
    while (!Serial.available());             // wait for input
    inString[inCount] = Serial.read();       // get it
    if (inString [inCount] == INTERMINATOR) break;  //it is the terminator?  //MANCA IL CONTROLLO SULLA LUNGHEZZA!!
    // CI VUOLE UN OR tipo "or inCount == INLENGTH"
  } 
  while(++inCount < INLENGTH);
  inString[inCount] = 0;                     // null terminate the string
  //Serial.print(F("R1 "));  //Only for debug
  //Serial.println(inString);
  comm=inString[0];
}

void Blink(){
  // Serial.println(F("Blink LED"));  //Only for debug
  digitalWrite(LEDPIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);               // wait for a second
  digitalWrite(LEDPIN, LOW);    // turn the LED off by making the voltage LOW
}


void PrintMenu(){
  Serial.println(F("1 1 One step"));
  Serial.println(F("2 1 -One step"));
  Serial.println(F("3 1 10 step"));
  Serial.println(F("4 1 -10 step"));
  Serial.println(F("5 1 50 step"));
  Serial.println(F("6 1 -50 step"));

  Serial.println(F("a 2 One step"));
  Serial.println(F("b 2 -One step"));
  Serial.println(F("c 2 10 step"));
  Serial.println(F("d 2 -10 step"));
  Serial.println(F("e 2 50 step"));
  Serial.println(F("f 2 -50 step"));

  Serial.println(F("v Print version"));
  Serial.println(F("B Blink LED 13"));
  Serial.println(F("m print menu"));
  Serial.println(F("--------------------"));
  Serial.println(F("Type number and press enter"));
}

void Step(){
  digitalWrite(STEP1PULSE, HIGH);
  delayMicroseconds(DELAY);          
  digitalWrite(STEP1PULSE, LOW); 
  delayMicroseconds(DELAY);
}

void Step2(){
  digitalWrite(STEP2PULSE, HIGH);
  delayMicroseconds(DELAY);          
  digitalWrite(STEP2PULSE, LOW); 
  delayMicroseconds(DELAY);
}

void Steps(int passi, int dir){
  if (dir == 1)
  {
    digitalWrite(STEP1DIR, HIGH);
  }
  else
  {
    digitalWrite(STEP1DIR, LOW);
  }
  for (int i=0; i <= passi; i++){
    Step();
    //delay(10);
  }
}

void Steps2(int passi, int dir){
  if (dir == 1)
  {
    digitalWrite(STEP2DIR, HIGH);
  }
  else
  {
    digitalWrite(STEP2DIR, LOW);
  }
  for (int i=0; i <= passi; i++){
    Step2();
    //delay(10);
  }
}

void ParseMenu(char Stringa){
  //Serial.println(F("Parsing")); //Only for debug
  switch (Stringa) {
  case '1':
    Steps(1,0);
    break;
  case '2':
    Steps(1,1);
    break;
  case '3':
    Steps(10,0);
    break;
  case '4':
    Steps(10,1);
    break;
  case '5':
    Steps(50,0);
    break;
  case '6':
    Steps(50,1);
    break;
  case 'a':
    Steps2(1,0);
    break;
  case 'b':
    Steps2(1,1);
    break;
  case 'c':
    Steps2(10,0);
    break;
  case 'd':
    Steps2(10,1);
    break;
  case 'e':
    Steps2(50,0);
    break;
  case 'f':
    Steps2(50,1);
    break;
  case 'm':
    PrintMenu();
    break;
  case 'B':
    Blink();
    break;
  case 'v':
    PrintVersion();
    break;
  default:
    Serial.print(F("Command Unknown! ->"));
    Serial.println(Stringa,HEX);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Stepper05SerialControl2");  
  pinMode(STEP1DIR, OUTPUT);     
  pinMode(STEP1PULSE, OUTPUT);
  pinMode(STEP2DIR, OUTPUT);     
  pinMode(STEP2PULSE, OUTPUT);
  digitalWrite(STEP1DIR, LOW);
  digitalWrite(STEP1PULSE, LOW);
  digitalWrite(STEP2DIR, LOW);
  digitalWrite(STEP2PULSE, LOW);
}

void loop() {
  GetCharFromSerial();
  //Serial.println(inString);
  ParseMenu(comm);
}
