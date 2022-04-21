#pragma once
#include "common.h"
#include "SnObject.h"
#include "ConnectObj.h"

enum ESessionState
{
    ESessionState_NONE = 0,
    ESessionState_CONNECT = 1,
    ESessionState_LOGIN = 2
};

class Session
{
public:
    Session(const ConnectObjPtr& conn, uint64 now);

    ESessionState& state() { return _state; }
    std::string& account() { return _account; }
    uint64 SN() const { return _conn.lock()->GetSN(); }

    void HandleMessage(const ConnectObjPtr& conn, const BufferPtr& buffer, const MsgHeader* header);
    void SendMsg(const std::string& msg);
private:
    std::weak_ptr<ConnectObj> _conn;
    uint64 _createTime = 0;
    ESessionState _state = ESessionState_NONE;
    std::string _account;
};