#pragma once
#include "SrxlDevice.hpp"

class RemoteReceiver : public SrxlMaster {
  struct CH_HANDLER {
    bool enabled;
    int32_t val;
  };

private:
  CH_HANDLER channels[32];

protected:
  byte transmitter_id;
  void event_on_finished_handshake(bool result) override {
    Serial.printf("RR::on_finished_handshake(%d)\n", result);
    if (result) {
      transmitter_id = slaves[0];
    }
  }
  uint32_t build_ch_mask() {
    uint32_t mask = 0;
    uint32_t flag = 1;
    for (int i = 0; i < 32; i++) {
      if (channels[i].enabled) {
        mask |= flag;
      }
      flag << 1;
    }
    return mask;
  }

  uint16_t convert_to_srxl(int32_t v) {
    return 0x8000;
  }

  void event_on_before_sending_msg(SrxlPacketBuffer &msg) override {
    SrxlMaster::event_on_before_sending_msg(msg);
    Serial.printf("RemoteReceiver::event_on_before_sending_msg\n");
    if (state == State::CONTROL) {
      msg.clear();
      msg.p_type = PT_CONTROL;
      byte cmd = 0;
      byte reply_id = 0;
      byte rssi = 0;
      uint16_t frameLosses = 0;
      msg.write(cmd);
      msg.write(reply_id);
      msg.write(rssi);
      msg.write(frameLosses);
      uint32_t mask = build_ch_mask();
      msg.write(mask);
      for (int i = 0; i < 32; i++) {
        if (channels[i].enabled) {
          msg.write((channels[i].val));
        }
      }
      msg.pack();
    }
  }

public:
  RemoteReceiver(SoftwareSerial &serial, SrxlDeviceID id, int time_frame = 22)
    : SrxlMaster(serial, id, time_frame) {}
};
