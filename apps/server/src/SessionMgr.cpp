#include "SessionMgr.h"

std::shared_ptr<Session>& SessionMgr::CreateSession(const ConnectObjPtr& conn , uint64 now)
{
    assert(_sessionMgr.find(conn->GetSN()) == _sessionMgr.end());
    std::shared_ptr<Session> session = std::make_shared<Session>(conn, now);
    session->state() = ESessionState_CONNECT;
    _sessionMgr[conn->GetSN()] = session;
    LOG_INFO("CreateSession: sessionMgr size: %lu", _sessionMgr.size());
    return _sessionMgr[conn->GetSN()];
}

void SessionMgr::RemoveSession(uint64 sn)
{
    auto it = _sessionMgr.find(sn);
    assert(it != _sessionMgr.end());
    it->second->state() = ESessionState_NONE;
    _sessionMgr.erase(it);
    LOG_INFO("RemoveSession: sessionMgr size: %lu", _sessionMgr.size());
}