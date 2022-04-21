#include "zinx.h"
#include "msg.pb.h"
#include <google/protobuf/message.h>
#include <list>

class GameMsg : public UserData
{
public:
    enum MSG_TYPE
    {
        MSG_TYPE_LOGIN_ID_NAME = 1,
        MSG_TYPE_CHAT_CONTENT = 2,
        MSG_TYPE_NEW_POSTION = 3,
        MSG_TYPE_BROADCAST = 200,
        MSG_TYPE_LOGOFF_ID_NAME = 201,
        MSG_TYPE_SRD_POSTION = 202
    } enMsgType;
    
    //已知消息内容创建消息对象
    GameMsg(MSG_TYPE type, google::protobuf::Message* pMsg);
    //将字节流内容转换成消息结构
	GameMsg(MSG_TYPE type, std::string stream);

    virtual ~GameMsg();

    std::string serialize();

    google::protobuf::Message* _pMsg = nullptr;
};

class MultiMsg : public UserData
{
public:
    std::list<GameMsg *> m_Msgs;
};