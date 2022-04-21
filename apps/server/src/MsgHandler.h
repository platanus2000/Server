#pragma once
#include "common.h"
#include "PacketDispatcher.h"

class MsgHandler : public IPacketDispatcher
{
public:
    MsgHandler();
    void RegisterHandler(uint16 msgID, Handler handler) override;
    bool DispatchPacket(NetPacket& pNetPacket) override;

    void OnLogin(NetPacket& packet);
private:
    std::map<uint16, Handler> _funcMgr;
};