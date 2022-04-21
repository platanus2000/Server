#include "GameChannel.h"
#include "GameRole.h"
#include "zinx.h"

GameChannel::GameChannel(int fd) : ZinxTcpData(fd)
{
    
}

GameChannel::~GameChannel()
{
    
}

AZinxHandler* GameChannel::GetInputNextStage(BytesMsg& input)
{
    return m_proto;
}

ZinxTcpData* GameConnFact::CreateTcpDataChannel(int fd)
{
    //创建tcp通道对象
    auto pChannel = new GameChannel(fd);
    //创建协议对象
    auto pProtocol = new GameProtocol();
    //创建玩家对象
    auto pRole = new GameRole();
    //绑定协议对象和通道对象
    pChannel->m_proto = pProtocol;
    pProtocol->m_channel = pChannel;
    //绑定协议对象和玩家对象
    pProtocol->m_pRole = pRole;
    pRole->m_pProto = pProtocol;
    //将协议对象添加到kernel
    ZinxKernel::Zinx_Add_Proto(*pProtocol);
    ZinxKernel::Zinx_Add_Role(*pRole);
    
    return pChannel;
}