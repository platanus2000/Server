#include "Map.h"
#include "msg.pb.h"

void Map::Init(int xbegin, int xend, int ybegin, int yend, int xcount, int ycount)
{
    std::unique_ptr<AOIWorld> aoi(new AOIWorld(xbegin, xend, ybegin, yend, xcount, ycount));
    _aoi = std::move(aoi);
}

void Map::AddPlayer(const std::shared_ptr<Player>& player, const Pos& pos)
{
    player->SetMap(shared_from_this());
    player->SetPos(pos);
    _aoi->AddObj(player.get());
    //通知周围玩家
    pb::Player py;
    py.set_pid(0);
    py.set_username(player->name());
    py.mutable_pos()->set_x(player->GetX());
    py.mutable_pos()->set_y(player->GetY());
    BroadcastSrdPlayer(py.SerializeAsString(), player->GetPos());
}

void Map::DelPlayer(const std::shared_ptr<Player>& player)
{
    _aoi->DelObj(player.get());
}

void Map::BroadcastSrdPlayer(const std::string& msg, const Pos& pos)
{
    std::list<GridObj*> objs = _aoi->GetSrdObj(pos);
    for (auto& obj : objs)
    {
        if (Player* player = obj->ToPlayer())
        {
            player->GetSession()->SendMsg("");
        }
    }
}