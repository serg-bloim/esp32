const int pin = 34;
void setup() {
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  int adcVal = analogRead(pin);
  Serial.printf("Value: %d \n", adcVal);
  delay(500);
}
