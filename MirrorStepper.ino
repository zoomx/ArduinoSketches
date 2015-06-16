/*
Stepper04
 Added serial control
 Stepper Motor Control
 Move the motor depending on commands received
 Using EasyDriver stepper module
 
 2015 02 25 by zoomx
 */

#define LEDPIN 13  //Led is on pin 13 in Arduino UNO boards.

//Serial input with termination section
#define INLENGTH 5          //Max string lenght over serial. Needed for input with termination
#define INTERMINATOR 13     //GetCharFromSerial routine cycle waiting for this character. Needed for input with termination
char inString[INLENGTH+1];  //GetCharFromSerial returns this char array. Needed for input with termination
char comm;  //First character received. Needed for input with termination

int Distance = 0;  // Record the number of steps we've taken

void PrintVersion(){
  Serial.println("Stepper04");
  Serial.print(F("Version 0.1"));
  Serial.print(__DATE__);  //this is the compiling date
  Serial.print(F(" "));
  Serial.println(__TIME__); //this is the compiling time
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
  Serial.println(inString);
  comm=inString[0];
}

void Blink(){
  // Serial.println(F("Blink LED"));  //Only for debug
  digitalWrite(LEDPIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);               // wait for a second
  digitalWrite(LEDPIN, LOW);    // turn the LED off by making the voltage LOW
}


void PrintMenu(){
  Serial.println(F("1 One step"));
  Serial.println(F("2 -One step"));
  Serial.println(F("3 10 step"));
  Serial.println(F("4 -10 step"));
  Serial.println(F("5 50 step"));
  Serial.println(F("6 -50 step"));
  //Serial.println(F("9 SD write test"));
  Serial.println(F("v Print version"));
  Serial.println(F("B Blink LED 13"));
  Serial.println(F("--------------------"));
  Serial.println(F("Type number and press enter"));
}

void Step(){
  digitalWrite(9, HIGH);
  delayMicroseconds(2000);          
  digitalWrite(9, LOW); 
  delayMicroseconds(2000);
}

void Steps(int passi, int dir){
  if (dir == 1)
  {
    digitalWrite(8, HIGH);
  }
  else
  {
    digitalWrite(8, LOW);
  }
  for (int i=0; i <= passi; i++){
    Step();
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
  Serial.println("Stepper04");  
  pinMode(8, OUTPUT);     
  pinMode(9, OUTPUT);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
}

void loop() {
  PrintMenu();
  GetCharFromSerial();
  Serial.println(inString);
  ParseMenu(comm);
}
