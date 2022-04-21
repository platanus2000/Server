#pragma once
#include "AOIWorld.h"
#include "SnObject.h"
#include "common.h"
#include "Player.h"

struct MapInfo
{
    int32 id = 0;
    int32 type = 0;
    int32 height = 0;
    int32 width = 0;
};

class Map : public SnObject, public std::enable_shared_from_this<Map>
{
public:
    void Init(int xbegin, int xend, int ybegin, int yend, int xcount, int ycount);

    void AddPlayer(const std::shared_ptr<Player>& player, const Pos& pos);
    void DelPlayer(const std::shared_ptr<Player>& player);
    void BroadcastSrdPlayer(const std::string& msg, const Pos& pos);
private:
    std::unique_ptr<AOIWorld> _aoi;
    std::unique_ptr<MapInfo> _mapInfo;
};