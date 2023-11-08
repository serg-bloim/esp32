
#include "USB.h"
#include "USBHID.h"
#include "USBHIDKeyboard.h"

int pins[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,21,33,34,35,36,37,38,39,40};
const int PIN_A = 18;
const int PIN_B = 33;
USBHIDKeyboard Keyboard;
HardwareSerial& SerialPort = Serial;
void setup() {
  // pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  Serial.begin(9600, SERIAL_8N1, 1, 2);
  led_start();
  ledcSetup(0, 5000, 8);
  ledcSetup(1, 5000, 8);
  ledcAttachPin(PIN_A, 0);
  ledcAttachPin(PIN_B, 1);
}

void loop() {
  Serial.println("Phase 1");
  // digitalWrite(PIN_A, HIGH);
  for(int i = 0; i <= 255; i++){
  ledcWrite(0, i);
  delay(4);
  }
  // delay(1000);
  Serial.println("Phase 2");
  for(int i = 0; i <= 255; i++){
  ledcWrite(1, i);
  delay(4);
  }
  Serial.println("Phase 3");
  // digitalWrite(PIN_A, LOW);
    for(int i = 255; i >= 0; i--){
  ledcWrite(0, i);
  delay(4);
  }
  // delay(1000);
  Serial.println("Phase 4");
  for(int i = 255; i >= 0; i--){
    ledcWrite(1, i);
    delay(4);
  }
}
