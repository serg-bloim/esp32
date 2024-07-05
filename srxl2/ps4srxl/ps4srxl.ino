#include "StaticConfig.h"
#include "SrxlDevice.hpp"
#include <CRC16.h>
#include <SoftwareSerial.h>

SoftwareSerial serial;
SrxlDevice srxl(serial, DEVICES_REMOTE_RECEIVER);

void setup() {
  serial.begin(115200, EspSoftwareSerial::Config::SWSERIAL_8N1, SRXL_PIN);
}

void loop() {
  srxl.update();
}
