#include "Robot.h"
#include "Logger.h"
#include <syscall.h>
#include <arpa/inet.h>

Robot::Robot(const std::string& ip, uint16 port) :
_network(std::make_shared<Network>()),
_connector(std::make_shared<NetworkConnector>(_network))
{
    _connector->Connect(ip, port);
}

void Robot::Start()
{

}

void Robot::Update()
{
    _connector->Update();
}

void Robot::NotifyAll(uint16 msgID, const std::string& str)
{
    //LOG_INFO("Robot %ld Send Msg: %s", syscall(SYS_gettid), str.c_str());
    _network->SendAll(msgID, str);
}