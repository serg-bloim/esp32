#ifndef SrxlDevice_H
#define SrxlDevice_H
#include <Arduino.h>
#include "utils.hpp"
#include "SoftwareSerial.h"
#include "ByteBuffer.hpp"
#include "SrxlCommon.hpp"
#include "SrxlStreamReader.hpp"

SrxlDeviceID handshake_device_ids[] = {1, 2, 0x31, 0x31};
enum SRXL_STATE
{
    HANDSHAKE,
};
class SrxlDevice
{
private:
    SRXL_STATE state;
    SoftwareSerial &serial;
    unsigned int time_frame;
    unsigned long time_frame_start;
    unsigned long time_frame_next;
    uint8_t handshake_ind;
    SrxlDeviceID id;
    bool has_next_msg = false;
    SrxlStreamReader ssr;
    SrxlPacketBuffer next_msg;
    byte baud_rate = 0;
    byte info = 0;
    uint32_t uid = 0x524753;
    static void noop(SrxlPacketBuffer &, SrxlDevice &dev) {}
    static void noop(SrxlGenericPack &, SrxlDevice &dev) {}

public:
    void (*event_on_before_sending_msg)(SrxlPacketBuffer &, SrxlDevice &dev) = SrxlDevice::noop;
    void (*event_on_after_sending_msg)(SrxlPacketBuffer &, SrxlDevice &dev) = SrxlDevice::noop;
    void (*event_on_received_msg)(SrxlGenericPack &, SrxlDevice &dev) = SrxlDevice::noop;
    SrxlDevice(SoftwareSerial &serial, SrxlDeviceID id, int time_frame = 22) : serial(serial),
                                                                               time_frame(time_frame),
                                                                               handshake_ind(0),
                                                                               id(id)
    {
        this->time_frame_start = millis();
        this->time_frame_next = time_frame_start + time_frame;
    }
    void update()
    {
        if (serial.available())
        {
            ssr.append(serial.read());
            if (ssr.msg_ready())
            {
                SrxlGenericPack msg;
                ssr.read_msg_into(msg);
                event_on_received_msg(msg, *this);
            }
        }
        auto now = millis();
        if (now > time_frame_next)
        {
            next_msg.clear();
            if (event_on_before_sending_msg)
            {
                event_on_before_sending_msg(next_msg, *this);
            }
            do_send();
            if (event_on_after_sending_msg)
            {
                event_on_after_sending_msg(next_msg, *this);
            }
        }
    }
    void handshake()
    {
        if (this->message_queue_len() == 0)
        {
            next_msg.write(this->id);
            auto dst_id = handshake_device_ids[handshake_ind++];
            next_msg.write(dst_id);
            byte priority = 0x10;
            next_msg.write(priority);
            next_msg.write(this->baud_rate);
            next_msg.write(this->info);
            next_msg.write(this->uid);
            next_msg.pack();
            if (handshake_ind >= arraySize(handshake_device_ids))
            {
                handshake_ind = 0;
            }
        }
    }
    size_t do_send()
    {
        serial.enableTx(true);
        serial.write(next_msg.c_str(), next_msg.len());
        serial.enableTx(false);
    }
    uint32_t message_queue_len()
    {
        return has_next_msg ? 1 : 0;
    }
};

class SrxlMaster : SrxlDevice{

};
#endif // SrxlDevice_H
