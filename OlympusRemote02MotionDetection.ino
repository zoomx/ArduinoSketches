/*
http://luckylarry.co.uk/arduino-projects/arduino-motion-triggered-camera/
 
 zoomx@ tiscalinet. it
 20141103
 changed IR pin pinIRLED from 13 to 12
 changed pinLED from 9 to 13
 
 20141222
 changer IR library
 added serial output for debugging
 
 Original
 LUCKYLARRY.CO.UK - IR Remote control for Nikon using Arduino with motion trigger
 Uses a PIR sensor to trigger the camera remote/ IR led if a change in background
 temperature is detected.
 
 */

#include <multiCameraIrControl.h>

#define IR_LED 12
#define STATUS_LED 13
#define PIR 4

int currentState = 0;                                   // set a variable to store a count.

Olympus E500(IR_LED);

void setup() {
  int i;

  pinMode(IR_LED, OUTPUT);                            // set the pin as an output
  pinMode(STATUS_LED, OUTPUT);                              // set the LED pin as ouput
  pinMode(PIR, INPUT);                               // set the PIR pin as an input


  Serial.begin(9600);
  Serial.println("Olympus Remote 02");

  Serial.println("Five blinks");
  for(i=0;i<5;i++){
    //Five blinks
    digitalWrite(STATUS_LED, HIGH);
    digitalWrite(IR_LED, HIGH);
    Serial.println("HIGH");
    delay(500);
    digitalWrite(STATUS_LED, LOW);
    digitalWrite(IR_LED, LOW);
    Serial.println("LOW");
    delay(500);
  }
}

void loop() {

  if ((digitalRead(PIR) == HIGH) && (currentState <= 2)) { // count to limit the taking of photos
    Serial.println("Movement detected->SHOOT!");
    E500.shutterNow();                                      // take the picture
    digitalWrite(STATUS_LED, HIGH);                         // turn LED on
  } 
  else {
    Serial.println("NO Movement");
    digitalWrite(STATUS_LED, LOW);
    currentState = 0;                                   //reset the count when the PIR is off.
  }

  currentState++;
  delay(5000);                                          // delay for 2 seconds - 2 seconds between taking photos if the PIR is active for more than 2 seconds.
}




