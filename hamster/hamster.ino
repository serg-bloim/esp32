#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "ASUS";
const char* password = "A4388Ed8843";
const char* api_key = "7AKI7ZVF3RPEIS2Y";

const int pin = 34;
const int treshold = 2048;
bool state = false;
bool has_state_changed = false;
int rotations = 0;

void setup() {
  Serial.begin(9600);
  delay(1000);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected");
}

void loop() {
  processState();
  if(has_state_changed){
    Serial.printf("State: %d \n", state);
    if(state == 0){
      // The signal has just been HIGH and wend back LOW
      rotations++;
      Serial.printf("Rotations: %d \n", rotations);
      if(WiFi.status() == WL_CONNECTED){
        HTTPClient http;
        String url = String("https://api.thingspeak.com/update?api_key=") + api_key + "&field2=" + String(rotations);
        http.begin(url);
        int status_code = http.GET();
        Serial.printf("Backend response: %d\n", status_code);
        http.end();
      }else{
        Serial.println("WiFi not connected");
      }
    }
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