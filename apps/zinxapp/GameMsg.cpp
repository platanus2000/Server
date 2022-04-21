#include "GameMsg.h"

using namespace pb;

GameMsg::GameMsg(MSG_TYPE type, google::protobuf::Message* pMsg) : enMsgType(type), _pMsg(pMsg)
{

}

GameMsg::GameMsg(MSG_TYPE type, std::string stream)
{
    //通过简单工厂构造具体的消息对象
    switch (type)
    {
    case MSG_TYPE_LOGIN_ID_NAME:
        _pMsg = new SyncPid();
        break;
    case MSG_TYPE_CHAT_CONTENT:
        _pMsg = new Talk();
        break;
    case MSG_TYPE_NEW_POSTION:
        _pMsg = new Position();
        break;
    case MSG_TYPE_BROADCAST:
        _pMsg = new BroadCast();
        break;
    case MSG_TYPE_LOGOFF_ID_NAME:
        _pMsg = new SyncPid();
        break;
    case MSG_TYPE_SRD_POSTION:
        _pMsg = new SyncPlayers();
        break;
    default:
        break;
    }

    if (_pMsg)
        //将参数解析成消息对象
        _pMsg->ParseFromString(stream);
}

GameMsg::~GameMsg()
{

}

std::string GameMsg::serialize()
{
    return _pMsg->SerializeAsString();
}