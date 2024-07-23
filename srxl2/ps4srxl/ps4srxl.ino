#include "StaticConfig.h"
// #include "RemoteReceiver.hpp"
// #include <SoftwareSerial.h>
#include "utils.hpp"
#include "Blinker.hpp"
#include "Ps4Client.hpp"

// SoftwareSerial serial;
// RemoteReceiver srxl(serial, DEVICES_REMOTE_RECEIVER);
Blinker blk(2, 500);
Ps4Client ps4;

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("setup 1");
  blk.begin();
  // srxl.begin(SRXL_PIN);
  ps4.begin("a8:2b:b9:45:ac:89");
  Serial.println("Start");
}

void loop() {
  Serial.println("loop");
  blk.update();
  // srxl.set_channel(0, convert_unsigned(ps4.throttle));
  // srxl.set_channel(1, convert_signed(ps4.pitch));
  // srxl.set_channel(2, convert_signed(ps4.roll));
  // srxl.set_channel(3, convert_signed(ps4.yaw));
  // // srxl.set_channel(4, ps4.safe_mode);
  // // srxl.set_channel(5, ps4.panic_mode);
  // // srxl.set_channel(6, ps4.ch7);
  // srxl.update();
  delay(1000);
}

void on_ps4_event(){
}
const int32_t MIN_RANGE = -0xFFFF;
const int32_t MAX_RANGE = +0xFFFF;
int32_t convert_unsigned(int32_t v){
  return map(cap(v, MIN_RANGE, MAX_RANGE), 0, MAX_RANGE, MIN_RANGE, MAX_RANGE);
}

int32_t convert_signed(int32_t v){
  return v;
}

