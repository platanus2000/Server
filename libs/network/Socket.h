#pragma once
#include <sys/socket.h>
#include <string>
#include "common.h"

class sockaddr_in;

class Socket
{
public:
    Socket(SOCKET fd);
    ~Socket();

    void Bind(const std::string& addr, uint16 port);
    void Listen();
    int32 Accept(sockaddr_in& addr);
    bool Connect(const std::string& ip, uint16 port);
    void Close();

    void OpenReuse();
    void NoBlocking();

public:
    SOCKET fd() const { return _sockfd; }
protected:
    SOCKET _sockfd = INVALID_SOCKET;
};