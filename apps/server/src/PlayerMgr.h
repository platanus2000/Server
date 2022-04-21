#pragma once
#include "common.h"
#include "Player.h"

class PlayerMgr
{
public:
    std::unique_ptr<Player>& CreatePlayer(std::shared_ptr<Session>& session, std::string account);
private:
    std::unordered_map<uint64, std::unique_ptr<Player>> _players;
};