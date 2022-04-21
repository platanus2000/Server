#pragma once
#include "common.h"
#include "Map.h"

class MapMgr
{
public:
    void Update();
    std::unique_ptr<Map>& CreateMap();
private:
    std::unordered_map<uint64, std::unique_ptr<Map>> _maps;
};