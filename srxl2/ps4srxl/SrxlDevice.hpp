#ifndef SrxlDevice_H
#define SrxlDevice_H
#include <Arduino.h>
#include "utils.hpp"
#include "SoftwareSerial.h"
#include "ByteBuffer.hpp"
#include "SrxlCommon.hpp"
#include "SrxlStreamReader.hpp"
#include "List.hpp"

SrxlDeviceID handshake_device_ids[] = { 1, 2, 0x31, 0x31 };

class SrxlDevice {
private:
  SoftwareSerial &serial;
  unsigned int time_frame;
  unsigned long time_frame_start;
  unsigned long time_frame_next;
  SrxlStreamReader ssr;
  SrxlPacketBuffer next_msg;

protected:
  SrxlDeviceID id;
  uint32_t uid = 0x524753;
  byte baud_rate = 0;
  byte info = 0;
  byte priority = 0x10;

  virtual void event_on_before_sending_msg(SrxlPacketBuffer &) {}
  virtual void event_on_after_sending_msg(SrxlPacketBuffer &) {}
  virtual void event_on_received_msg(SrxlGenericPack &) {}

public:
  SrxlDevice(SoftwareSerial &serial, SrxlDeviceID id, int time_frame = 22)
    : serial(serial),
      time_frame(time_frame),
      id(id) {
    this->time_frame_start = millis();
    this->time_frame_next = time_frame_start + time_frame;
  }
  void update() {
    if (serial.available()) {
      ssr.append(serial.read());
      if (ssr.msg_ready()) {
        SrxlGenericPack msg;
        ssr.read_msg_into(msg);
        event_on_received_msg(msg);
      }
    }
    auto now = millis();
    if (now > time_frame_next) {
      next_msg.clear();
      event_on_before_sending_msg(next_msg);
      do_send();
      event_on_after_sending_msg(next_msg);
    }
  }
  size_t do_send() {
    serial.enableTx(true);
    serial.write(next_msg.c_str(), next_msg.len());
    serial.enableTx(false);
  }
};

class SrxlMaster : SrxlDevice {
private:
protected:
  enum State {
    HANDSHAKE,
    CONTROL,
  };
  State state = HANDSHAKE;
  uint8_t handshake_ind = 0;
  StaticList<byte, 255> slaves;
  void event_on_before_sending_msg(SrxlPacketBuffer &msg) override {
    SrxlMaster &master = *this;
    switch (master.state) {
      case HANDSHAKE:
        master.handshake(msg);
        break;
      case CONTROL:
        break;
      default:
        break;
    }
  }
  void event_on_after_sending_msg(SrxlPacketBuffer &msg) override {
    SrxlMaster &master = *this;
    if (handshake_ind == 0) {
      event_on_finished_handshake(state == State::CONTROL);
    }
  }
  void event_on_received_msg(SrxlGenericPack &msg) override {
    SrxlMaster &master = *this;
    switch (msg.type) {
      case PT_HANDSHAKE:
        // auto hs = static_cast<SrxlHandshakePack&>(msg);
        auto hs = (SrxlHandshakePack &)(msg);
        master.on_received_handshake(hs);
        break;

        // default:
        //     break;
    }
  }
  virtual void event_on_finished_handshake(bool result) {}

  void handshake(SrxlPacketBuffer &msg) {
    if (handshake_ind == arraySize(handshake_device_ids) && slaves.size() > 0) {
      state = State::CONTROL;
      byte dst_id = 0xFF;
      msg.p_type = PT_HANDSHAKE;
      msg.write(this->id);
      msg.write(dst_id);
      byte priority = this->priority;
      msg.write(priority);
      msg.write(this->baud_rate);
      msg.write(this->info);
      msg.write(this->uid);
      msg.pack();
    } else {
      byte dst_id = handshake_device_ids[handshake_ind++];
      msg.p_type = PT_HANDSHAKE;
      msg.write(this->id);
      msg.write(dst_id);
      byte priority = this->priority;
      msg.write(priority);
      msg.write(this->baud_rate);
      msg.write(this->info);
      msg.write(this->uid);
      msg.pack();
    }
    if (handshake_ind == arraySize(handshake_device_ids)) {
      handshake_ind = 0;
    }
  }
  void on_received_handshake(SrxlHandshakePack &msg) {
    if (!slaves.contains(msg.data.src_id)) {
      slaves.add(msg.data.src_id);
    }
  }
  void reset() {
    state = HANDSHAKE;
    handshake_ind = 0;
    slaves.clear();
  }
public:
  SrxlMaster(SoftwareSerial &serial, SrxlDeviceID id, int time_frame = 22)
    : SrxlDevice(serial, id, time_frame) {
    reset();
  }
};
#endif  // SrxlDevice_H
