#include <LedControl.h>
//datain, clk, cs
LedControl lc = LedControl(12,11,10,1); //(DIN,CLOCK,LOAD,number of chips)

int powerPin1 = 5; //buttons
int powerPin2 = A3; //max7219

//button pins
int nextPin = 7;
boolean nextToggle = false;

int resetPin = 8;
boolean resetToggle = false;
float resetTimer;
boolean resetRep = false; //prevents pressing button twice

int autoPin = 6;
boolean autoToggle = false;
boolean autoNext = true; //false if manual next, true if automatic next was false
float autoTimer;

int board[] =        {
  0,0,0,0,0,0,0,0, //oscillator, beacon
  0,0,0,0,0,0,0,0,
  0,0,1,1,0,0,0,0,
  0,0,1,0,0,0,0,0,
  0,0,0,0,0,1,0,0,
  0,0,0,0,1,1,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0};

byte startingBoards[5][64]= {
  {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,1,1,0,0,0,
    0,0,0,1,1,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0  }
  ,

  {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,1,1,1,1,0,0,
    0,0,1,0,0,1,0,0,
    0,0,1,0,0,1,0,0,
    0,0,1,1,1,1,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0  }
  ,

  {
    0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,1,0,
    0,1,0,0,0,0,1,0,
    0,1,0,0,0,0,1,0,
    0,1,0,0,0,0,1,0,
    0,1,0,0,0,0,1,0,
    0,1,1,1,1,1,1,0,
    0,0,0,0,0,0,0,0  }
  ,

  {
    1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1  }
  ,

  {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0  }
};





int oldboard[64]; //previous board, crosschecked with main for repeating gens
int ss; //number of same squares between the two
int border[100]; //board with infinite or dead border

byte com[6][64] = {  
  {
    0,0,0,0,0,0,0,0, //oscillator, beacon
    0,0,0,0,0,0,0,0,
    0,0,1,1,0,0,0,0,
    0,0,1,0,0,0,0,0,
    0,0,0,0,0,1,0,0,
    0,0,0,0,1,1,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0  }
  ,

  {
    0,0,0,0,0,0,0,0, //oscillator, toad
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,1,1,1,0,0,
    0,0,1,1,1,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0  }
  ,

  {
    0,0,0,0,0,0,0,0, //mold
    0,0,0,0,1,1,0,0,
    0,0,0,1,0,0,1,0,
    0,1,0,0,1,0,1,0,
    0,0,0,0,0,1,0,0,
    0,1,0,1,1,0,0,0,
    0,0,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0  }
  ,

  {
    0,1,0,0,0,0,0,0, //glider
    0,0,1,0,0,0,0,0,
    1,1,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0  }
  ,

  {
    0,0,1,1,1,1,0,0, //smiley face
    0,1,0,0,0,0,1,0,
    1,0,1,0,0,1,0,1,
    1,0,0,0,0,0,0,1,
    1,0,1,0,0,1,0,1,
    1,0,0,1,1,0,0,1,
    0,1,0,0,0,0,1,0,
    0,0,1,1,1,1,0,0  }
  ,

  {
    0,0,0,0,0,0,0,0, //heart
    0,1,1,0,1,1,0,0,
    1,0,0,1,0,0,1,0,
    1,0,0,0,0,0,1,0,
    1,0,0,0,0,0,1,0,
    0,1,0,0,0,1,0,0,
    0,0,1,0,1,0,0,0,
    0,0,0,1,0,0,0,0  }
};

int comcount = 0;

int checkco[] = {
  -11,-10,-9,-1,1,9,10,11}; //surrounding cells of each member of core border.  i-11,i-10,i-9...i+11.
int boardmap[] = {
  11,12,13,14,15,16,17,18, //to help with board/border conversion
  21,22,23,24,25,26,27,28,
  31,32,33,34,35,36,37,38,
  41,42,43,44,45,46,47,48,
  51,52,53,54,55,56,57,58,
  61,62,63,64,65,66,67,68,
  71,72,73,74,75,76,77,78,
  81,82,83,84,85,86,87,88};
int pop; //pop of surrounding units
int genpop; //pop of each generation
float firstgen; //pop of first generation

int c01;  //something i forget

int stager = 0; //reset button
float timer = 0; //set to millis, if res btn is pressed for more than 1s, combinations
int stagen = 0; //next button
float timen = 0; //set to millis.  if btn is pressed for greater than timen==700, toggle automatic next every interval
float timeM = 0; //for timed MAIN .  checks if an interval has passed, to move, if automatic next is enabled
int interval = 700; //time interval for button and timed next

int count = 0; //generation
int border0 = 1; //if 0, dead border.  if 1, infinte border.
int repeat = 0; //if 0, gen is new.  if 1, gen has been repeated

void setup(){
  Serial.begin(9600);
  Serial.println("Life 8x8");
  //power pin
  pinMode(powerPin1,OUTPUT);
  pinMode(powerPin2,OUTPUT);
  digitalWrite(powerPin1,HIGH);
  digitalWrite(powerPin2,HIGH);
  //buttons
  pinMode(nextPin,INPUT);
  pinMode(resetPin,INPUT);
  pinMode(autoPin,INPUT);
  //for the matrix
  lc.shutdown(0, false); //turns on display
  lc.setIntensity(0,8); //15 maximum brightness
  lc.clearDisplay(0);  //clears display
  lc.setScanLimit(0,7); ///how many digits there are.
  randomSeed(A4);
  //lc.setLed(0,48%8,48/8,true); //board, column, row
  start(30); //starting animation, input is interval between frames.
  start(30);
  start(30);
  set(0); //0 to generate random board, 1 to generate current (above) board
};

void loop(){
  prnt();
  Serial.println(autoNext);
  //reset button
  if(digitalRead(resetPin)==HIGH && resetToggle == false){
    resetToggle = true;
    resetTimer = millis();
  };
  //next preset board
  if(digitalRead(resetPin)==HIGH && resetToggle == true && millis() >= resetTimer + interval){
    if(resetRep == false){
      resetTimer = millis();
      resetRep = true;
      for(int i = 0; i < 64; i++){
        board[i] = com[comcount][i];
      };
      set(1);
      autoTimer = millis();
      comcount++;
      if(comcount == 6){
        comcount = 0;
      };
    };
    if(resetRep == true && millis() >= resetTimer + interval){
      resetRep = false;
    };
  };
  //generate a random board
  if(digitalRead(resetPin)==LOW && resetToggle == true && millis() < resetTimer + interval && resetRep == false){
    resetToggle = false;
    autoTimer = millis();
    set(0);
  };
  if(digitalRead(resetPin)==LOW && resetRep==true){
    resetRep = false;
    resetToggle = false;
  };
  //next button
  if(digitalRead(nextPin)==HIGH && nextToggle==false){
    nextToggle = true;
    autoTimer = millis();
    test();
  };
  if(digitalRead(nextPin)==LOW && nextToggle==true){
    nextToggle = false;
  };
  //automatic next toggle button
  if(digitalRead(autoPin)==HIGH && autoToggle==false){
    autoToggle = true;
    autoTimer = millis();
    autoNext = true;   //was !autoNext;
    Serial.println("Pressed");
  };
  if(digitalRead(autoPin)==LOW){
    autoToggle = false;
  };
  //automatic next
  if(autoNext == true && millis() >= autoTimer + interval){
    Serial.println("next");
    test();
    autoTimer = millis();
  };
};
//starting animation
void start(int x){
  for(int j; j <64; j++){
    lc.setLed(0,j%8,j/8,startingBoards[0][j]);
  };
  delay(x);
  for(int j; j <64; j++){
    lc.setLed(0,j%8,j/8,startingBoards[1][j]);
  };
  delay(x);
  for(int j; j <64; j++){
    lc.setLed(0,j%8,j/8,startingBoards[2][j]);
  };
  delay(x);
  for(int j; j <64; j++){
    lc.setLed(0,j%8,j/8,startingBoards[3][j]);
  };
  delay(x);
};
//print function
void prnt(){
  for(int i = 0; i < 64; i++){
    lc.setLed(0,i%8,i/8,board[i]);
  };
};
//generates a random board/sets current board
void set(int x){
  if(x==0){
    count = 0;
    repeat = 0;
    randomSeed(analogRead(A5));
    for(int i = 0; i < 64; i++){ //set every space either 1 or 0, as random as possible.
      board[i] = random(2);
    };
    borderset();
    Serial.print("generation ");
    Serial.println(count);
    Serial.print("population: ");
    firstgen = gen(2); //returns pop of 1st gen, for future ratios
    Serial.println(firstgen);
  };
  if(x==1){ //if 1 sets current board, as a starting board.  i.e. any of the presets or 
    //thematrix assigned to int board[] at the top
    count = 0;
    repeat = 0;
    //board is already set
    borderset();
    Serial.print("generation ");
    Serial.println(count);
    Serial.print("population: ");
    firstgen = gen(2);
    Serial.println(firstgen);
  };
};
//new generation!
void test(){
  //map old board
  for(int y = 0; y < 64; y++){
    oldboard[y] = board[y];
  };
  //derive next generation
  for(int i = 0; i < 64; i++){
    c01 = 0;
    if (c01 == 0 && board[i] == 1){
      pop = 0;
      for (int h = 0; h < 8; h++){
        if (border[boardmap[i] + checkco[h]] == 1){
          pop++;
        };
      };
      if (pop == 0 || pop == 1){
        board[i] = 0;
      };
      if (pop == 2 || pop == 3){
        board[i] = 1;
      };
      if (pop > 3){
        board[i] = 0;
      };
      c01 = 1;
    };
    if(c01 == 0 && board[i] == 0){
      pop = 0;
      for (int h = 0; h < 8; h++){
        if (border[boardmap[i] + checkco[h]] == 1){
          pop++;
        };
      };
      if (pop == 3){
        board[i] = 1;
      }
      else{
        board[i] = 0;
      };
      c01 = 1;
    };
    c01 = 0;
    //update board
  };
  borderset(); //after next generation is derived, give it a border, either infinite or
  //dead. test for repeat
  ss = 0;
  for(int y = 0; y< 64; y++){
    if(oldboard[y] == board[y]){
      ss++;
    };
    if(ss > 63){
      repeat = 1;
    };
  };
  if(repeat == 0){ //if not a repeat, continue
    count++;
    Serial.print("generation ");
    Serial.println(count);
    Serial.print("population: ");
    Serial.println(gen(0));
    Serial.print("Surviving ratio: ");
    Serial.println(gen(1));
  };
  if(repeat == 1){ //if repeat, stop and show stats of final generation
    stager = 0; //?debuging?
    Serial.print("final generation ");
    Serial.println(count);
    Serial.print("population: ");
    Serial.println(gen(0));
    Serial.print("Surviving ratio: ");
    Serial.println(gen(1));
  };
};
void borderset(){
  for(int k = 0; k < 100; k++){
    border[k] = 0;
  }; //start every int off as 0, for dead border by default
  for(int l = 1; l < 9; l++){ //row by row, net generation is placed into the center
    border[l*10+1] = board[(l-1)*8];
    border[l*10+2] = board[(l-1)*8+1];
    border[l*10+3] = board[(l-1)*8+2];
    border[l*10+4] = board[(l-1)*8+3];
    border[l*10+5] = board[(l-1)*8+4];
    border[l*10+6] = board[(l-1)*8+5];
    border[l*10+7] = board[(l-1)*8+6];
    border[l*10+8] = board[(l-1)*8+7];
  };
  if(border0 == 1){ //if border is infinite...
    for(int l = 0; l < 8; l++){
      border[1+l] = board[56+l];
      border[91+l] = board[0+l];
      border[10+l*10] = board[7+8*l];
      border[20+l*10-1]= board[0+l*8];
    };
    //corners, which were too difficult to put in the for loop
    border[0] = board[63];
    border[9] = board[56];
    border[90] = board[7];
    border[99] = board[0];
  };
};
float gen(int x){ //0 yields population, 1 yields ratio, 2 yields firstgen population (only used once). the others are used at the end of each new generation.
  if(x == 0 || x == 1){
    genpop = 0;
    for(int i = 0; i < 64; i++){
      if (board[i] == 1){
        genpop++;
      };
    };
    if(x == 0){
      return genpop;
    };
  };
  if(x == 1){
    return genpop/firstgen;
  };
  if(x == 2){
    firstgen = 0;
    for(int i = 0; i < 64; i++){
      if (board[i] == 1){
        firstgen++;
      };
    };
    return firstgen;
  };
};


