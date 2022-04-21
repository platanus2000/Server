#include "Socket.h"
#include "Logger.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

Socket::Socket(SOCKET fd)
{
    _sockfd = fd;
}

Socket::~Socket()
{
    Close();
    _sockfd = 0;
}

void Socket::Bind(const std::string& ip, uint16 port)
{
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
    if (::bind(_sockfd, (sockaddr*)&addr, sizeof(sockaddr)) < 0)
    {
        perror("bind error");
        LOG_FATAL("bind error, address: %s, port: %d, errno: %d", ip.c_str(), port, errno);
        Close();
        exit(1);
    }
    LOG_INFO("Bind Success, ip: %s, port: %d", ip.c_str(), port);
}

void Socket::Listen()
{
    if (::listen(_sockfd, MAX_CLIENT) < 0)
    {
        perror("listen error");
        LOG_FATAL("listen error, errno: %d", errno);
        Close();
        exit(1);
    }
    LOG_INFO("Listen Success...");
}

int32 Socket::Accept(sockaddr_in& addr)
{
    socklen_t len = sizeof(addr);
    int clientfd = ::accept(_sockfd, (sockaddr*)&addr, &len);
    if (clientfd == INVALID_SOCKET) 
	{
#if __LINUX__
		switch (errno) 
		{
            case EWOULDBLOCK : 

            case ECONNRESET :
            case ECONNABORTED :
            case EPROTO :
            case EINTR :
                // from UNIX Network Programming 2nd, 15.6
                // with nonblocking-socket, ignore above errors

            case EBADF : 
            case ENOTSOCK : 
            case EOPNOTSUPP : 
            case EFAULT : 

            default :
                {
                    break;
                }
		}
#endif
    }
    if (clientfd <= 0)
    {
        perror("accept error");
        LOG_FATAL("accept error, errno: %d", errno);
    }
    else
    {
        LOG_INFO("Accept Success, ip: %s, port: %d", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    }
    return clientfd;
}

bool Socket::Connect(const std::string& ip, uint16 port)
{
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
    int ret = ::connect(_sockfd, (sockaddr*)&addr, sizeof(sockaddr));
    // bool isNoblock = fcntl(_sockfd, F_GETFL) & O_NONBLOCK;
    // if (!isNoblock && ret < 0)
    if (ret != 0 && errno != EINPROGRESS)   //非阻塞用EINPROGRESS
    {  
        perror("connect error");
        LOG_WARN("connect error, address: %s, port：%d, errno: %d", ip.c_str(), port, errno);
        return false;
    }
    else
    {
        LOG_INFO("Waiting Connect Complete..., ip: %s, port: %d", ip.c_str(), port);
    }
    return true;
}

void Socket::Close()
{
    if (_sockfd == 0)
    {
        LOG_FATAL("close error");
    }
    ::close(_sockfd);
    LOG_INFO("close fd: %d", _sockfd);
}

void Socket::OpenReuse()
{
    int32 reuse = 1;
    if (::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0)
    {
        perror("set reuse error");
        LOG_FATAL("set reuse error ..., errno: %d", errno);
        Close();
        exit(1);        
    }
    LOG_INFO("server setreuse");
}

void Socket::NoBlocking()
{
    int opts = fcntl(_sockfd, F_GETFL);
    if (opts < 0)
    {
        perror("GETFL error");
        LOG_FATAL("fcntl(serverfd_,GETFL, errno: %d", errno);
        exit(1);
    }
    if (fcntl(_sockfd, F_SETFL, opts | O_NONBLOCK) < 0)
    {
        perror("SETFL error");
        LOG_FATAL("fcntl(serverfd_,SETFL,opts), errno: %d", errno);
        exit(1);
    }
    LOG_INFO("server setnonblocking...");
}
