#pragma once
#include "common.h"
#include "NetworkConnector.h"
#include "Network.h"

class Robot
{
public:
    Robot(const std::string& ip, uint16 port);
    void Start();
    void Update();
    void NotifyAll(uint16 msgID, const std::string& str);
private:
    std::shared_ptr<Network> _network;
    std::shared_ptr<NetworkConnector> _connector;
};