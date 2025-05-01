#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "ASUS";
const char* password = "A4388Ed8843";
const char* api_key = "7AKI7ZVF3RPEIS2Y";
const int backend_field_rotation = 1;
const int backend_field_rpm = 2;
const int backend_field_errs = 3;
const bool http_on = true;

const int pin = 34;
const int ledPin = 2;
const int treshold = 1;
bool state = false;
bool has_state_changed = false;
int rotations = 0;
int network_errors = 0;
void setup() {
  pinMode(pin, INPUT);
  pinMode (ledPin, OUTPUT);
  Serial.begin(9600);
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

void loop() {
  processState();
  checkLed();
  if(has_state_changed){
    Serial.printf("State: %d \n", state);
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
  int adcVal = digitalRead(pin);
  // int adcVal = 100;
  bool new_state = adcVal < treshold;
  // bool new_state = true;
  has_state_changed = new_state != state;
  state = new_state;
  return has_state_changed;
}