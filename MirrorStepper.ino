/*
Stepper01
http://www.schmalzhaus.com/EasyDriver/Examples/EasyDriverExamples.html

*/
void setup() {
  Serial.begin(9600);
  Serial.println("Stepper01");
  pinMode(8, OUTPUT);     
  pinMode(9, OUTPUT);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
}

void loop() {
  digitalWrite(9, HIGH);
  delay(1);          
  digitalWrite(9, LOW); 
  delay(1);          
}

