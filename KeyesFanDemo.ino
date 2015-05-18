/*
KeyesFanDemo

Demo of the Keyes fan board

*/
int INA = 9;
int INB = 8;
void setup() {
  Serial.begin(9600);
  pinMode(INA,OUTPUT);
  pinMode(INB,OUTPUT);
  Serial.println("Keyes Fan Demo");
}
void loop() {
  digitalWrite(INA,LOW);
  digitalWrite(INB,HIGH);
  Serial.println("INA LOW  INB HIGH");
  delay(1000);
  digitalWrite(INA,HIGH);
  digitalWrite(INB,LOW);
  Serial.println("INA HIGH INB LOW");
  delay(1000);
}

