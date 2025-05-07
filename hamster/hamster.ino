#include <WiFi.h>
#include <HTTPClient.h>
#include "utils.h"

const char* ssid = "ASUS";
const char* password = "A4388Ed8843";
const char* api_key = "UQH38MC1XHXBTAML";
const int backend_field_rotation = 1;
const int backend_field_rpm = 2;
const int backend_field_errs = 3;
const bool http_on = true;

const int pin = 36;
const int ledPin = 2;
const int treshold = 30;
const int buffer_treshold = 10;
bool state = false;
bool has_state_changed = false;
int rotations = 0;
int network_errors = 0;
void setup() {
  pinMode(pin, INPUT);
  pinMode (ledPin, OUTPUT);
  Serial.begin(115200);
  delay(1000);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected");
  rotations = readLastValueFromBackend();
  Serial.printf("Backend rotations: %d\n", rotations);
  updateBackend();
}

RunningMean pin_mean(5);
RunningMean neutral_state(5000, analogRead(pin));

int lvl, mean_lvl;
void loop() {
  processState();
  checkLed();
  // Serial.printf("State: %d, Lvl: %d, Mean_lvl: %d, Neutral_state: %d, Rots: %d \n", state, lvl, mean_lvl, neutral_state.get(), rotations);
  if(has_state_changed){
    // Serial.printf("State: %d, Lvl: %d, Mean_lvl: %d \n", state, lvl, mean_lvl);
    if(state == 0){
      // The signal has just been HIGH and wend back LOW
      rotations++;
      fleshLed();
      Serial.printf("Rotations: %d \n", rotations);
    }
  }
  delay(0);
}
unsigned long led_on_until = 0;
void fleshLed(){
  led_on_until = millis() + 100;
}
void checkLed(){
  int lvl = LOW;
  if(millis() < led_on_until) lvl = HIGH;
  digitalWrite(ledPin, lvl);
}
void updateBackend(){
  xTaskCreatePinnedToCore(
  updateBackendTask,     // Task function.
    "UpdateBackendTask", // name of task.
      10000,             // Stack size of task
       NULL,             // parameter of the task
          1,             // priority of the task 0 - 3
       NULL,             // Task handle to keep track of created task
         0);             // pin task to core X   
}

unsigned long next_available_run = 0;
unsigned long last_sample_ms = 0;
void updateBackendTask(void * pvParameters){
  int last_rotations = rotations;
  for(;;){
      int rotation_delta = rotations - last_rotations;
      unsigned long now = millis();
      unsigned long time_delta = now - last_sample_ms;
      last_sample_ms = now;
      last_rotations = rotations;
      int rpm = rotation_delta * 60 * 1000 / time_delta;
      if(http_on && WiFi.status() == WL_CONNECTED){
        unsigned long start = millis();
        HTTPClient http;
        String url = String("https://api.thingspeak.com/update?api_key=") + api_key
         + "&field"+String(backend_field_rotation)+"=" + String(last_rotations)
         + "&field"+String(backend_field_rpm)+"=" + String(rpm)
         + "&field"+String(backend_field_errs)+"=" + String(network_errors);
        http.begin(url);
        int status_code = http.GET();
        Serial.printf("Setting rotations to %d, rpm = %d. Backend response: %d\n", last_rotations, rpm, status_code);
        http.end();
        unsigned long end = millis();
        Serial.printf("Time took for http: %d\n", end - start);
      }else{
        Serial.println("WiFi not connected");
        network_errors++;
        WiFi.begin(ssid, password);
      }
      Serial.println("Finished http");
    int delay = 15000;
    int till_next_run = delay - (millis() % delay);
    vTaskDelay(till_next_run);
  }
}

int readLastValueFromBackend(){
  int backend_rotations = 0;
  if(http_on && WiFi.status() == WL_CONNECTED){
    unsigned long start = millis();
    HTTPClient http;
    String url = String("https://api.thingspeak.com/channels/2940912/fields/") + String(backend_field_rotation)+"/last.txt?api_key="+api_key;
    http.begin(url);
    int status_code = http.GET();
    Serial.printf("Backend response: %d\n", status_code);
    if(status_code == 200){
      String payload = http.getString();
      backend_rotations = payload.toInt();
    }
    http.end();
    unsigned long end = millis();
    Serial.printf("Time took for http: %d\n", end - start);
  }else{
    Serial.println("WiFi not connected");
  }
  return backend_rotations;
}
bool processState(){
  lvl = analogRead(pin);
  pin_mean.add(lvl);
  neutral_state.add(lvl);
  mean_lvl = pin_mean.get();
  auto diff = abs(mean_lvl - neutral_state.get());
  bool new_state = diff > treshold;
  bool is_buffer_zone = abs(diff - treshold) < buffer_treshold;
  has_state_changed = new_state != state && !is_buffer_zone;
  if(has_state_changed) state = new_state;
  return has_state_changed;
}