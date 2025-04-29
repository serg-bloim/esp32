#ifndef ByteBuffer_H
#define ByteBuffer_H

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
    size_t len()
    {
        return pos;
    }
    void clear()
    {
        pos = 0;
    }
};

template <size_t BUF_SIZE>
class CycledByteBuffer
{
private:
    byte buf[BUF_SIZE];
    size_t start = 0;
    size_t end = -1;

public:
    CycledByteBuffer() {}
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
            memcpy(buf, &data + part1_len, sz - part1_len);
        }
        return sz;
    }
    size_t data_size()
    {
        size_t len = end - start;
    }
    void normalize()
    {
#define NORMALIZE_N(var)      \
    if (end >= BUF_SIZE)      \
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
        size_t len = end - start;
        return len;
    }
    void clear()
    {
        start = 0;
        end = 0;
        // start = 0;
    }
    template <typename T>
    size_t readInto(T &dst)
    {
        size_t sz = sizeof(T);
        if (sz > len())
        {
            return 0;
        }

        size_t part1_len = min(BUF_SIZE - end, sz);
        size_t part2_len = sz - part1_len;

        memcpy(dst, buf + start, part1_len);
        if (part2_len)
        {
            memcpy(dst + part1_len, buf, part2_len);
        }
        start += sz;
        normalize();
    }
};
void testCycledByteBuffer(){
    
}
#endif // ByteBuffer_H
