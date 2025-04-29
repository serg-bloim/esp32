#pragma once
#include <PS4Controller.h>
#include <functional>
#include "utils.hpp"

class Ps4Client;
void on_packet_received_global();
Ps4Client* inst2 = nullptr;

class Ps4Client {
public:
  int32_t throttle;
  int32_t pitch;
  int32_t roll;
  int32_t yaw;
  Ps4Client()
    : ps4controller(PS4) {
    inst2 = this;
  }
  void begin(const char* mac) {
    ps4controller.begin(mac);
    // auto f = on_packet_received_global;
    ps4controller.attach(on_packet_received_global);
  }
  void on_packet_received() {
    // Serial.printf("ps4controller addr = %08X\n", ps4controller);
    throttle = parseThrottle();
    pitch = parseSlider(ps4controller.RStickY());
    roll = parseSlider(ps4controller.RStickX());
    yaw = parseSlider(ps4controller.LStickX());
    // Serial.printf("Packet received throttle: % 6d, pitch: % 6d, roll: % 6d, yaw: % 6d\n", throttle, pitch, roll, yaw);
  }
protected:
private:
  PS4Controller& ps4controller;
  int32_t parseThrottle(){
    return parseSlider(ps4controller.LStickY());
  }
  int32_t parseSlider(int8_t v){
    v = cap(v, -127, 127);
    return map(v, -127, 127, -0xFFFF, 0xFFFF);
  }
};


void on_packet_received_global(){
  if(inst2){
  inst2->on_packet_received();
  }
}