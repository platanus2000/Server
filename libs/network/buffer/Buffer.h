#pragma once
#include <assert.h>
#include <string>
#include <vector>

/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
///
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode

class Buffer
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize)
    : m_buffer(kCheapPrepend + initialSize)
    , m_readIndex(kCheapPrepend)
    , m_writeIndex(kCheapPrepend)
    {
    }

    size_t readableBytes() const { return m_writeIndex - m_readIndex; }
    size_t writableBytes() const { return m_buffer.size() - m_writeIndex; }
    size_t prependableBytes() const { return m_readIndex; }

    char* beginRead() { return begin() + m_readIndex; }
    const char* beginRead() const { return begin() + m_readIndex; }
    char* beginWrite() { return begin() + m_writeIndex; }
    const char* beginWrite() const { return begin() + m_writeIndex; }

    void hasWritten(size_t len)
    {
        assert(len <= writableBytes());
        m_writeIndex += len;
    }

    const char* peek() const { return begin() + m_readIndex; }

    void retrieve(size_t len)
    {
        assert(len <= readableBytes());
        if (len < readableBytes())
        {
            m_readIndex += len;
        }
        else
        {
            retrieveAll();
        }
    }

    void retrieveAll()
    {
        m_readIndex = kCheapPrepend;
        m_writeIndex = kCheapPrepend;
    }

    std::string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }

    std::string retrieveAsString(size_t len)
    {
        assert(len <= readableBytes());
        std::string result = std::string(peek(), len);
        retrieve(len);
        return result;
    }

    void ensureWritableBytes(size_t len)
    {
        if (writableBytes() < len)
        {
            makeSpace(len);
        }
        assert(writableBytes() >= len);
    }

    void makeSpace(size_t len)
    {
        if (writableBytes() + prependableBytes() < len + kCheapPrepend)
        {
            m_buffer.resize(m_writeIndex + len);
        }
        else
        {
            // move readable data to the front, make space inside buffer
            assert(kCheapPrepend < m_readIndex);
            size_t readable = readableBytes();
            std::copy(begin() + m_readIndex,
                      begin() + m_writeIndex,
                      begin() + kCheapPrepend);
            m_readIndex = kCheapPrepend;
            m_writeIndex = m_readIndex + readable;
            assert(readable == readableBytes());
        }
    }

    void append(const char* data, size_t len)
    {
        ensureWritableBytes(len);
        std::copy(data, data + len, begin() + m_writeIndex);
        hasWritten(len);
    }

    ssize_t readFd(int fd, int* savedErrno);

private:
    char* begin() { return &*m_buffer.begin(); }
    const char* begin() const { return &*m_buffer.begin(); }

private:
    std::vector<char> m_buffer;
    size_t m_readIndex = kCheapPrepend;
    size_t m_writeIndex = kCheapPrepend;
};