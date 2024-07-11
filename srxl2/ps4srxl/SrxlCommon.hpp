#ifndef SrxlCommon_H
#define SrxlCommon_H
#include "ByteBuffer.hpp"
#include "crc.hpp"

using SrxlByteBuffer = StaticByteBuffer<100>;
using SrxlDeviceID = byte;
const byte SRXL_HEADER_START = 0xA6;
const byte SRXL_PTYPE_HANDSHAKE = 0x21;
const SrxlDeviceID DEVICES_REMOTE_RECEIVER = 0x10;
const SrxlDeviceID DEVICES_TRANSMITTER = 0x31;

enum SRXL_PACKET_TYPE : byte{
    PT_HANDSHAKE = 0x21,
    PT_CONTROL = 0xCD,
};
template <typename T>
void little2big_endian(T &v)
{
    byte *buf = (byte *)&v;
    auto sz = sizeof(T);
    for (int i = sz / 2 - 1; i >= 0; i--)
    {
        byte a = buf[i];
        buf[i] = buf[sz - i - 1];
        buf[sz - i - 1] = a;
    }
}

class SrxlPacketBuffer : public SrxlByteBuffer
{
    protected:
    void reset(){
        write(SRXL_HEADER_START);
        write(p_type);
        write((byte)0);
    }
public:
    byte p_type = SRXL_PTYPE_HANDSHAKE;
    SrxlPacketBuffer(byte p_type = 0) : SrxlByteBuffer(), p_type(p_type)
    {
        // Write header stub so write() will do right into the data section
        reset();
    }
    const byte *get_buffer()
    {
        return c_str();
    }
    size_t pack()
    {
        writeAt(0, SRXL_HEADER_START);
        writeAt(1, p_type);
        byte len = this->len();
        writeAt(2, (byte)(len+2)); // +2 for

        Crc16XModem crc;

        crc.add(c_str(), len);
        uint16_t crcVal = crc.calc();
        little2big_endian(crcVal);
        write(crcVal);
        return 0;
    }
    void clear() override{
        SrxlByteBuffer::clear();
        reset();
    }
};
template<typename T>
struct __attribute__((packed)) SrxlTypedPack
{
    byte type;
    byte len;
    uint16_t crc;
    T data;
};
using SrxlGenericPack = SrxlTypedPack<byte[75]>;
struct __attribute__((packed)) PL_HANDSHAKE
{
    byte src_id;
    byte dst_id;
    byte priority;
    byte baud;
    byte info;
    uint32_t uid;
};

using SrxlHandshakePack = SrxlTypedPack<PL_HANDSHAKE>;
#endif // SrxlCommon_H
