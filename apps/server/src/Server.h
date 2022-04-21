#pragma once
#include "common.h"
#include "NetworkListener.h"
#include "Network.h"
#include "ThreadObj.h"
#include "Session.h"
#include "PacketDispatcher.h"
#include <queue>
#include "CASQueue.h"


class Server : public ThreadObj
{
public:
    Server(const std::string& ip, uint16 port, std::unique_ptr<IPacketDispatcher>& dispatcher);
    void Update() override;
    void HandleMessage(const ConnectObjPtr& conn, const BufferPtr& buffer, const MsgHeader* header);
    void HandleConnectSuccess(const ConnectObjPtr& conn);
    void HandleCloseSuccess(const ConnectObjPtr& conn);

public:
    uint64 now() const { return _now; }

private:
    uint64 _now = 0;
private:
    std::shared_ptr<Network> _network;
    std::shared_ptr<NetworkListener> _listener;
    std::deque<NetPacket> _recvQueue;
    std::deque<NetPacket> _dispatchQueue;
    // CASQueue<NetPacket> _recvQueue;
    // CASQueue<NetPacket> _dispatchQueue;

    std::mutex _mtx;
    std::unique_ptr<IPacketDispatcher> _dispatcher;
};