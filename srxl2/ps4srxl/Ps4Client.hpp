#pragma once
#include <PS4Controller.h>
#include <functional>


class Ps4Client {
public:
  int32_t throttle;
  int32_t pitch;
  int32_t roll;
  int32_t yaw;
  Ps4Client()
    : ps4controller(PS4) {
    static Ps4Client* inst = this;
  }
  void begin(const char* mac) {
    ps4controller.begin(mac);
    auto f = on_packet_received_static;
    ps4controller.attach(f);
  }

protected:
private:
  static void on_packet_received_static() {
    static Ps4Client* inst;
    inst->on_packet_received();
  }
  PS4Controller& ps4controller;
  void on_packet_received() {
    throttle = parseThrottle();
    pitch = parseSlider(ps4controller.RStickY());
    roll = parseSlider(ps4controller.RStickX());
    yaw = parseSlider(ps4controller.LStickX());
  }
  int32_t parseThrottle(){
    return 0;
  }
  int32_t parseSlider(int8_t){
    return 0;
  }
};