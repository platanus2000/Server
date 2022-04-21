#pragma once
#include "ZinxTCP.h"
#include "GameProtocol.h"

class GameChannel : public ZinxTcpData
{
public:
    GameChannel(int fd);
    virtual ~GameChannel();
    GameProtocol *m_proto = nullptr;
    //返回协议对象
    virtual AZinxHandler *GetInputNextStage(BytesMsg &input) override;
};

class GameConnFact : public IZinxTcpConnFact
{
public:
    virtual ZinxTcpData *CreateTcpDataChannel(int fd) override;
};