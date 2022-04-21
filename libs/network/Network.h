#pragma once
#include <sys/epoll.h>
#include <unordered_map>
#include <functional>
#include "common.h"
#include "Socket.h"
#include "ConnectObj.h"
#include "NetworkListener.h"
#include "ThreadObj.h"

#define MAX_EVENT   5120

class NetworkListener;

class Network : public std::enable_shared_from_this<Network>, public ThreadObj
{
public:
    Network();
    ~Network();

    
    void AddEvent(int sockfd, int flag);
    void ModifyEvent(int sockfd, int flag);
    void DelEvent(int sockfd);

    virtual void Update();

    std::shared_ptr<ConnectObj>& CreateConnectObj(SOCKET socket, std::string ip, uint16 port);
    void Send(int sockfd, uint16 msgID, const std::string& data);
    void SendAll(uint16 msgID, const std::string& data);

    void HandleClose(const ConnectObjPtr& ptr);
private:


protected:
    std::unordered_map<SOCKET, std::shared_ptr<ConnectObj>> _connectMgr;

private:
    int _epfd = 0;
    struct epoll_event _events[MAX_EVENT] = {};
    int _mainSocketEventIndex = -1;
};