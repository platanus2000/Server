#pragma once
#include "Singleton.h"
#include "Session.h"
#include "Server.h"

class SessionMgr
{
public:
    std::shared_ptr<Session>& CreateSession(const ConnectObjPtr& conn, uint64 now);
    void RemoveSession(uint64 sn);
protected:
    std::unordered_map<uint64, std::shared_ptr<Session>> _sessionMgr;
private:
    friend class Session;
    friend class Server;
};