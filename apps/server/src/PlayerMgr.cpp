#include "PlayerMgr.h"

std::unique_ptr<Player>& PlayerMgr::CreatePlayer(std::shared_ptr<Session>& session, std::string account)
{
    std::unique_ptr<Player> player(new Player());
    player->_account = account;
    player->_session = session;
    _players[session->SN()] = std::move(player);
    return _players[session->SN()];
}