#pragma once
#include "common.h"
#include "ConnectObj.h"
#include "Network.h"

class NetworkConnector
{
public:
    NetworkConnector(const std::shared_ptr<Network>& network);
    bool Connect(std::string ip, uint16 port);
    void ConnectComplete();
    void ConnectSuccess();
    void Update();

public:
    void HandleRead();
private:
    std::weak_ptr<Network> _network;
    std::shared_ptr<ConnectObj> _connObj;
    bool _isConnected = false;
};