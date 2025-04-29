#pragma once

#include <CRC16.h>

class Crc16XModem
{
private:
  CRC16 crc = CRC16(CRC16_XMODEM_POLYNOME,
            CRC16_XMODEM_INITIAL,
            CRC16_XMODEM_XOR_OUT,
            CRC16_XMODEM_REV_IN,
            CRC16_XMODEM_REV_OUT);

public:
  Crc16XModem() {}
  void reset()
  {
    crc.restart();
  }
  uint16_t calc()
  {
    return crc.calc();
  }
  template <typename T>
  void add(T data)
  {
    add((byte *)&data, sizeof(T));
  }
  void add(const byte* data, size_t size)
  {
    crc.add(data, size);
  }
};
