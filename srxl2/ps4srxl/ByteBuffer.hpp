#ifndef ByteBuffer_H
#define ByteBuffer_H
#include "Arduino.h"
#include "usings.h"

class ByteBuffer
{
private:
    byte *buf;
    size_t size;
    size_t pos = 0;

public:
    ByteBuffer(byte *buf, size_t size) : buf(buf), size(size) {}
    template <typename T>
    ByteBuffer(T &buf) : ByteBuffer(reinterpret_cast<byte*>(std::addressof(buf)), sizeof(T)) {}
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

        if (pos >= size)
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
    size_t len()
    {
        return pos;
    }
    virtual void clear()
    {
        pos = 0;
    }
};

template <size_t BUF_SIZE>
class StaticByteBuffer : public ByteBuffer
{
private:
    byte buf[BUF_SIZE];

public:
    StaticByteBuffer() : ByteBuffer(buf, BUF_SIZE) {}
};

template <size_t BUF_SIZE>
class CycledByteBuffer
{
private:
    byte buf[BUF_SIZE];
    size_t start = 0;
    size_t end = 0;
    char stasBuf[50];
    char buf2[50];

public:
    CycledByteBuffer() {}

    char *stats()
    {
        for (int i = 0; i < BUF_SIZE; i++)
        {
            sprintf(buf2 + i * 2, "%02X", buf[i]);
        }
        buf2[BUF_SIZE * 2] = 0;

        sprintf(stasBuf, "start: %d, end: %d, len(): %d, data: %s", start, end, len(), buf2);
        return stasBuf;
    }
    template <typename T>
    size_t write(T data)
    {
        size_t sz = writeAt(end, data);
        end += sz;
        normalize();
        return sz;
    }
    template <typename T>
    size_t writeAt(size_t pos, T data)
    {
        size_t sz = sizeof(T);
        if (len() + sz > BUF_SIZE)
        {
            return 0;
        }
        if (end + sz <= BUF_SIZE)
        {
            memcpy(buf + end, &data, sz);
            return sz;
        }
        else
        {
            size_t part1_len = BUF_SIZE - end;
            memcpy(buf + end, &data, part1_len);
            memcpy(buf, ((byte *)&data) + part1_len, sz - part1_len);
        }
        return sz;
    }
    void normalize()
    {
#define NORMALIZE_N(var)      \
    if (var >= BUF_SIZE)      \
    {                         \
        var = var % BUF_SIZE; \
    }
        NORMALIZE_N(start)
        NORMALIZE_N(end)
    }
    const byte *c_str()
    {
        return buf;
    }
    size_t len()
    {
        if (end >= start)
            return end - start;
        else
            return end - start + BUF_SIZE;
    }
    void clear()
    {
        start = 0;
        end = 0;
        // start = 0;
    }
    byte peek(size_t offset = 0)
    {
        byte var;
        peekInto(var, offset);
        return var;
    }
    byte read()
    {
        byte var;
        readInto(var);
        return var;
    }
    template <typename T>
    size_t readInto(T &dst)
    {
        size_t sz = peekInto(dst);
        start += sz;
        normalize();
        return sz;
    }
    template <typename T>
    size_t peekInto(T &dst, size_t offset = 0)
    {
        size_t sz = sizeof(T);
        if (offset + sz > len())
        {
            Serial.printf("Error peekInto(sizeof(T)=%d, %d). len() = %d\n", sz, offset, len());
            return 0;
        }
        size_t read_pos = (start + offset) % BUF_SIZE;
        size_t part1_len = min(BUF_SIZE - read_pos, sz);
        size_t part2_len = sz - part1_len;

        memcpy(&dst, buf + read_pos, part1_len);
        if (part2_len)
        {
            memcpy(((byte *)&dst) + part1_len, buf, part2_len);
        }
        return sz;
    }
    class Iterator
    {
    private:
        CycledByteBuffer &parent;
        size_t i = 0;

    public:
        Iterator(CycledByteBuffer &parent) : parent(parent) {}
        bool has_next()
        {
            return i < parent.len();
        }
        byte next()
        {
            return parent.peek(i++);
        }
    };
    Iterator iterator()
    {
        Iterator iter(*this);
        return iter;
    }
};

#endif // ByteBuffer_H
