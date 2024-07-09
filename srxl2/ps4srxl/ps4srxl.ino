#include "StaticConfig.h"
#include "RemoteReceiver.hpp"
#include <SoftwareSerial.h>
#include "Blinker.hpp"

SoftwareSerial serial;
RemoteReceiver srxl(serial, DEVICES_REMOTE_RECEIVER);
Blinker blk(2, 500);

void setup() {
  blk.begin();
  serial.begin(115200, EspSoftwareSerial::Config::SWSERIAL_8N1, SRXL_PIN);
  Serial.begin(115200);
  delay(2000);
}

void loop() {
  blk.update();
  // srxl.update();
}
