 int Al;
 int led = 13;
 int xl;
 float vl;
 
 void setup() {
  // put your setup code here, to run once:
  pinMode(led,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(led, HIGH);
  xl = analogRead(Al);
  vl = xl * 5.0/1023.0;
  Serial.print("\nAnalog reading: ");Serial.print(xl);
  Serial.print("\nAnalog voltage: ");Serial.print(vl);
  delay(2000);
  digitalWrite(led, LOW);
  xl = analogRead(Al);
  vl = xl * 5.0/1023.0;
  Serial.print("\nAnalog reading: ");Serial.print(xl);
  Serial.print("\nAnalog voltage: ");Serial.print(vl);
  delay(2000);
}
