#include "MsgHandler.h"
#include "MsgID.pb.h"
#include "Login.pb.h"

using namespace Vera;

MsgHandler::MsgHandler()
{
    RegisterHandler(LoginReq_ID, std::bind(&MsgHandler::OnLogin, this, std::placeholders::_1));
}

void MsgHandler::RegisterHandler(uint16 msgID, Handler handler)
{
    assert(_funcMgr.find(msgID) == _funcMgr.end());
    _funcMgr[msgID] = handler;
}

bool MsgHandler::DispatchPacket(NetPacket& pNetPacket)
{
    auto it = _funcMgr.find(pNetPacket.msgID);
    if (it != _funcMgr.end())
    {
        it->second(pNetPacket);
        return true;
    }
    return false;
}

void MsgHandler::OnLogin(NetPacket& packet)
{
    LoginReq req;
    req.ParseFromArray(packet.buff.c_str(), packet.length);
    LOG_INFO("recv %s %s", req.account().c_str(), req.passward().c_str());
}