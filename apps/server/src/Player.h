#pragma once
#include "common.h"
#include "Session.h"
#include "AOIWorld.h"

class Map;
class Player : public GridObj
{
public:
    Player() {}
    ~Player() {}

    void Init();

    void SetMap(const std::shared_ptr<Map>& map) { _map = map; }
public:
    std::shared_ptr<Session>& GetSession() { return _session; }
    std::string name() const { return _name; }
public:
    std::string _account;
    std::string _name;
    uint64 _lastLoginTime;
    std::shared_ptr<Session> _session;
    std::weak_ptr<Map> _map;
};