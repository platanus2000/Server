#include "Session.h"
#include "SessionMgr.h"
#include "Login.pb.h"
#include "PlayerMgr.h"

using namespace Vera;

Session::Session(const ConnectObjPtr& conn, uint64 now) : _conn(conn), _createTime(now)
{
}

void Session::HandleMessage(const ConnectObjPtr& conn, const BufferPtr& buffer, const MsgHeader* header)
{
    LoginReq req;
    req.ParseFromArray(buffer->beginRead() + HEAD_SIZE, header->dataLen);
    auto& sessionMgr = Singleton<SessionMgr>::Instance()->_sessionMgr;
    auto it = sessionMgr.find(conn->GetSN());
    assert(it != sessionMgr.end());
    {
        it->second->state() = ESessionState_LOGIN;
        Singleton<PlayerMgr>::Instance()->CreatePlayer(it->second, req.account());
    }
    //LOG_INFO("Recv messgae: %s %s, msgID: %d", req.account().c_str(), req.passward().c_str(), header->msgID);
}

void Session::SendMsg(const std::string& msg)
{
    _conn.lock()->AddSendData(0, msg);
}