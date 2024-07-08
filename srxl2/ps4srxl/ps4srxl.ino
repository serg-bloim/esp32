#include "StaticConfig.h"
#include "RemoteReceiver.hpp"
#include <CRC16.h>
#include <SoftwareSerial.h>
#include "List.hpp"

SoftwareSerial serial;
RemoteReceiver srxl(serial, DEVICES_REMOTE_RECEIVER);

void setup() {
  serial.begin(115200, EspSoftwareSerial::Config::SWSERIAL_8N1, SRXL_PIN);
  Serial.begin(115200);
  delay(2000);
}

void loop() {
  srxl.update();
}
