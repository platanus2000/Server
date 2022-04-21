#include "Buffer.h"
#include <errno.h>
#include <sys/uio.h>

ssize_t Buffer::readFd(int fd, int* savedErrno)
{
    char extraBuf[65536];
    const size_t writable = writableBytes();
    struct iovec vec[2];
    vec[0].iov_base = beginWrite();
    vec[0].iov_len = writable;
    vec[1].iov_base = extraBuf;
    vec[1].iov_len = sizeof extraBuf;

    const int iovcnt = (writable < sizeof extraBuf) ? 2 : 1;
    const ssize_t n = readv(fd, vec, iovcnt);
    if (n < 0)
    {
        *savedErrno = errno;
    }
    else if (size_t(n) <= writable)
    {
        m_writeIndex += n;
    }
    else
    {
        m_writeIndex = m_buffer.size();
        append(extraBuf, n - writable);
    }
    return n;
}