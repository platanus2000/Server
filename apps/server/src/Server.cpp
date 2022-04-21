#include "Server.h"
#include "common.h"
#include "SessionMgr.h"
#include "ThreadMgr.h"
#include "PlayerMgr.h"

Server::Server(const std::string& ip, uint16 port, std::unique_ptr<IPacketDispatcher>& dispatcher) :
_network(std::make_shared<Network>()),
_listener(std::make_shared<NetworkListener>(_network, ip, port)),
_dispatcher(std::move(dispatcher))
{
    _listener->SetConnectCompleteCallback(std::bind(&Server::HandleConnectSuccess, this, std::placeholders::_1));
    _listener->SetCloseCompleteCallback(std::bind(&Server::HandleCloseSuccess, this, std::placeholders::_1));

    Singleton<ThreadMgr>::Instance()->AddObjToThread(_network);
}

void Server::Update()
{
    {
        std::lock_guard<std::mutex> lock(_mtx);
        std::swap(_recvQueue, _dispatchQueue);
    }
    for (auto& packet : _dispatchQueue)
    {
        _dispatcher->DispatchPacket(packet);
    }
    _dispatchQueue.clear();
    // while (!_recvQueue.Empty())     //TODO 会不会出现一些包很久才会被读到？
    // {
    //     NetPacket packet;
    //     _recvQueue.TryPop(packet);
    //     _dispatcher->DispatchPacket(packet);
    // }
}

void Server::HandleConnectSuccess(const ConnectObjPtr& conn)
{
    Singleton<SessionMgr>::Instance()->CreateSession(conn, _now);
    conn->SetMessageCallback(std::bind(&Server::HandleMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    //设置缓冲区大小
    uint32 recvLen = 128 * 1024;
    uint32 sendLen = 128 * 1024;
    setsockopt(conn->fd(), SOL_SOCKET, SO_RCVBUF, (const void*)&recvLen, sizeof(recvLen));
    setsockopt(conn->fd(), SOL_SOCKET, SO_SNDBUF, (const void*)&sendLen, sizeof(recvLen));
}

void Server::HandleMessage(const ConnectObjPtr& conn, const BufferPtr& buffer, const MsgHeader* header)
{
    std::lock_guard<std::mutex> lock(_mtx);
    _recvQueue.emplace_back(NetPacket(header->msgID, header->dataLen, buffer->beginRead() + HEAD_SIZE));
}

void Server::HandleCloseSuccess(const ConnectObjPtr& conn)
{
    Singleton<SessionMgr>::Instance()->RemoveSession(conn->GetSN());
}