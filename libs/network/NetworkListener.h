#pragma once
#include "common.h"
#include "Socket.h"
#include "ConnectObj.h"
#include <string>

class Network;

class NetworkListener
{
public:
    typedef std::function<void (SOCKET, const std::string&, uint16)> NewConnectionCallback;
    typedef std::function<void (const ConnectObjPtr&)> CompleteCallback;

    NetworkListener(const std::shared_ptr<Network>& network, const std::string& ip, uint16 port);
    void Listen();
    SOCKET fd() const { return _connObj->fd(); }

public:
    void SetNewConnectionCallback(const NewConnectionCallback& cb) { _newConnectionCallback = cb; }
    void SetConnectCompleteCallback(const CompleteCallback& cb) { _connectComplete = cb; }
    void SetCloseCompleteCallback(const CompleteCallback& cb) { _closeComplete = cb; }
    void SetMessageCallBack(const MessageCallBack& cb) { _messagecallback = cb; }

private:
    void handleRead();
    void handleNewConnection(SOCKET fd, const std::string& ip, uint16 port);
    void removeConnection(const ConnectObjPtr& conn);

private:
    std::weak_ptr<Network> _network;
    std::shared_ptr<ConnectObj> _connObj;
    NewConnectionCallback _newConnectionCallback;
    CompleteCallback _connectComplete;
    CompleteCallback _closeComplete;
    MessageCallBack _messagecallback;
};