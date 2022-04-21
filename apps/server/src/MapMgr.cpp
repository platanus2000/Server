#include "MapMgr.h"

void MapMgr::Update()
{
    
}

std::unique_ptr<Map>& MapMgr::CreateMap()
{
    std::unique_ptr<Map> map(new Map());
    uint64 id = map->GetSN();
    _maps[id] = std::move(map);
    return _maps[id];
}