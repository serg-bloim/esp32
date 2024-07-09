#include "StaticConfig.h"
#include "RemoteReceiver.hpp"
#include <SoftwareSerial.h>
#include "Blinker.hpp"

SoftwareSerial serial;
RemoteReceiver srxl(serial, DEVICES_REMOTE_RECEIVER,1000);
Blinker blk(2, 500);

void setup() {
  blk.begin();
  srxl.begin(SRXL_PIN);
  Serial.begin(115200);
  delay(2000);
}

void loop() {
  blk.update();
  srxl.update();
}
