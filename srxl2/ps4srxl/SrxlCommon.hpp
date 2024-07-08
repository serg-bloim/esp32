#ifndef SrxlCommon_H
#define SrxlCommon_H
#include "ByteBuffer.hpp"
#include <CRC16.h>
#include <CRC.h>

using SrxlByteBuffer = ByteBuffer<100>;
using SrxlDeviceID = byte;
const byte SRXL_HEADER_START = 0xA6;
const byte SRXL_PTYPE_HANDSHAKE = 0x21;
const SrxlDeviceID DEVICES_REMOTE_RECEIVER = 0x10;
const SrxlDeviceID DEVICES_TRANSMITTER = 0x31;

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
public:
    byte p_type;
    SrxlPacketBuffer(byte p_type = 0) : SrxlByteBuffer(), p_type(p_type)
    {
        // Write header stub so write() will do right into the data section
        write(SRXL_HEADER_START);
        write(SRXL_PTYPE_HANDSHAKE);
        write((byte)0);
    }
    const byte *get_buffer()
    {
        return c_str();
    }
    size_t pack()
    {
        writeAt(1, p_type);
        byte len = this->len();
        writeAt(2, len);

        CRC16 crc(CRC16_XMODEM_POLYNOME,
                  CRC16_XMODEM_INITIAL,
                  CRC16_XMODEM_XOR_OUT,
                  CRC16_XMODEM_REV_IN,
                  CRC16_XMODEM_REV_OUT);

        crc.add(c_str(), len);
        auto crcVal = crc.calc();
        little2big_endian(crcVal);
        write(crcVal);
    }
};
struct SrxlGenericPack
{
    byte type;
    byte len;
    byte data[75];
    uint16_t crc;
};

#endif // SrxlCommon_H
