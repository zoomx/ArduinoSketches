/*
Stepper03
 http://www.schmalzhaus.com/EasyDriver/Examples/EasyDriverExamples.html
 
 
 NON FUNZIONA CON LA SCHEDA RS e il motore RS
 */

#include <AccelStepper.h>

// Define a stepper and the pins it will use
AccelStepper stepper(1, 9, 8);

int pos = 600;

void setup()
{  
  Serial.begin(9600);
  Serial.println("Stepper03");  
  stepper.setMaxSpeed(20);
  stepper.setAcceleration(5);
  stepper.setMinPulseWidth(2000);
}

void loop()
{
  if (stepper.distanceToGo() == 0)
  {
    delay(500);
    pos = -pos;
    stepper.moveTo(pos);
  }
  stepper.run();
}
