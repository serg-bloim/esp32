#ifndef ByteBuffer_H
#define ByteBuffer_H
#include <Arduino.h>
#include "usings.h"

template <size_t BUF_SIZE>
class ByteBuffer
{
private:
    byte buf[BUF_SIZE];
    size_t pos = 0;

public:
    ByteBuffer() {}
    template <typename T>
    size_t write(T data)
    {
        size_t sz = writeAt(pos, data);
        pos += sz;
        return sz;
    }
    template <typename T>
    size_t writeAt(size_t pos, T data)
    {
        size_t sz = sizeof(T);
        if (pos >= BUF_SIZE)
        {
            return 0;
        }
        memcpy(buf + pos, &data, sz);
        return sz;
    }
    const byte *c_str()
    {
        return buf;
    }
    size_t len(){
        return pos;
    }
};
#endif // ByteBuffer_H
