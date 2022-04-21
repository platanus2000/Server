#include "NetworkListener.h"
#include "Network.h"
#include "Logger.h"
#include "ConnectObj.h"
#include <arpa/inet.h>
#include <thread>

NetworkListener::NetworkListener(const std::shared_ptr<Network>& network, const std::string& ip, uint16 port) :
    _network(network),
    _connObj(_network.lock()->CreateConnectObj(::socket(AF_INET, SOCK_STREAM, 0), ip, port))
{
    _connObj->socket().Bind(ip, port);
    _connObj->socket().OpenReuse();
    _connObj->socket().NoBlocking();
    _connObj->socket().Listen();
    _connObj->SetReadCallback(std::bind(&NetworkListener::handleRead, this));
    _newConnectionCallback = std::bind(&NetworkListener::handleNewConnection, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

void NetworkListener::handleRead()
{
    sockaddr_in clientaddr;
    SOCKET clientfd = _connObj->socket().Accept(clientaddr);
    if (clientfd > 0)
    {   
        std::string ip = inet_ntoa(clientaddr.sin_addr);
        uint16 port = ntohs(clientaddr.sin_port);
        if (_newConnectionCallback)
        {
            _newConnectionCallback(clientfd, ip, port);
        }
        else
        {
            LOG_FATAL("NetworkListener has null _newConnectionCallback");
        }
    }
    // else
    // {
    //     perror("NetworkListener Accept Error");
    //     LOG_FATAL("NetworkListener Accept Error");
    // }
}

void NetworkListener::handleNewConnection(SOCKET clientfd, const std::string& ip, uint16 port)
{
    auto& conn = _network.lock()->CreateConnectObj(clientfd, ip, port);
    conn->SetMessageCallback(_messagecallback);
    conn->SetCloseCallback(std::bind(&NetworkListener::removeConnection, this, std::placeholders::_1));
    if (_connectComplete)
    {
        _connectComplete(conn);
    }
}

void NetworkListener::removeConnection(const ConnectObjPtr& conn)
{
    _network.lock()->HandleClose(conn);
    if (_closeComplete)
    {
        _closeComplete(conn);
    }
    LOG_DEBUG("NetworkListener removeConnection, fd: %d", conn->fd());
}

