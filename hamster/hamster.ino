const int pin = 34;
const int treshold = 2048;
bool state = true;
bool has_state_changed = true;
void setup() {
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  processState();
  if(has_state_changed){
    Serial.printf("State: %d \n", state);
  }
  delay(50);
}

bool processState(){
  int adcVal = analogRead(pin);
  // int adcVal = 100;
  bool new_state = adcVal > treshold;
  // bool new_state = true;
  has_state_changed = new_state != state;
  state = new_state;
  return has_state_changed;
}