#ifndef SrxlDevice_H
#define SrxlDevice_H
#include <Arduino.h>
#include "SoftwareSerial.h"
#include "ByteBuffer.hpp"
#include "SrxlCommon.hpp"

SrxlDeviceID handshake_device_ids[] = {1, 2, 0x31,0x31};
enum SRXL_STATE{
    HANDSHAKE,
};
class SrxlDevice{
    private:
        SRXL_STATE state;
        SoftwareSerial& serial;
        unsigned int time_frame;
        unsigned long time_frame_start;
        unsigned long time_frame_next;
        uint8_t handshake_ind;
        SrxlDeviceID id;

    public:
    SrxlDevice(SoftwareSerial& serial, SrxlDeviceID id, int time_frame = 22): 
            serial(serial),
            time_frame(time_frame),
            handshake_ind(0),
            id(id){
        this->time_frame_start = millis();
        this->time_frame_next = time_frame_start + time_frame;
    }
    void update(){
        auto now = millis();
        if (state == HANDSHAKE){
            handshake();
        }
        if (now > time_frame_next)
        {
            /* code */
        }
        
    }
    void handshake(){
        if(this->message_queue_len() == 0){
            SrxlPacketBuffer msg_buffer;
            msg_buffer.write(this->id);
            this->queue_msg(msg_buffer);
        }
    }
    void queue_msg(SrxlPacketBuffer& buf){

    }
    uint32_t message_queue_len(){
        return 0;
    }
};
#endif // SrxlDevice_H

