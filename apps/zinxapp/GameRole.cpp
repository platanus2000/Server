#include "GameRole.h"

GameRole::GameRole()
{
}

GameRole::~GameRole()
{
}

bool GameRole::Init()
{
    return false;
}

//处理协议
UserData *GameRole::ProcMsg(UserData &_poUserData)
{
    return nullptr;
}

void GameRole::Fini()
{
}
