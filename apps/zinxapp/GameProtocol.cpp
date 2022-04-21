#include "GameProtocol.h"
#include <iostream>
#include "GameMsg.h"
#include "GameChannel.h"
#include "GameRole.h"

using namespace std;
using namespace pb;


GameProtocol::GameProtocol()
{
}


GameProtocol::~GameProtocol()
{
}

/*输入参数是通道传来的原始报文
返回值是转换后的消息对象MultiMsg
转换方式,TCP粘包处理*/
UserData * GameProtocol::raw2request(std::string _szInput)
{
    MultiMsg* pRet = new MultiMsg();
    szLast.append(_szInput);
    if (szLast.size() > 1024)
        szLast.clear();

    while (true)
    {
        if (szLast.size() < 8)
        {
            break;
        }
        //在前4个字节中读取消息内容长度
        int iLength = 0;
        iLength |= szLast[0];
        iLength |= szLast[1] << 8;
        iLength |= szLast[2] << 16;
        iLength |= szLast[3] << 24;

        int id = 0;
        id |= szLast[4];
        id |= szLast[5] << 8;
        id |= szLast[6] << 16;
        id |= szLast[7] << 24;

        //长度不够
        if (szLast.size()  - 8 < iLength)
        {
            break;
        }

        //构造消息
        pRet->m_Msgs.emplace_back(new GameMsg(GameMsg::MSG_TYPE(id), szLast.substr(8, iLength)));
        szLast.erase(szLast.begin(), szLast.begin() + iLength + 8);
    }

    for (auto& single : pRet->m_Msgs)
    {
        if (single->_pMsg)
            cout << single->_pMsg->Utf8DebugString() <<endl;
    }
    return pRet;
}


/*参数来自业务层，待发送的消息
返回值转换后的字节流*/
std::string * GameProtocol::response2raw(UserData & _oUserData)
{	
    int iLength = 0;
	int id = 0;
	std::string MsgContent;

	GET_REF2DATA(GameMsg, oOutput, _oUserData);
	id = oOutput.enMsgType;
	MsgContent = oOutput.serialize();
	iLength = MsgContent.size();

	auto pret = new std::string();

	pret->push_back((iLength >> 0) & 0xff);
	pret->push_back((iLength >> 8) & 0xff);
	pret->push_back((iLength >> 16) & 0xff);
	pret->push_back((iLength >> 24) & 0xff);
	pret->push_back((id >> 0) & 0xff);
	pret->push_back((id >> 8) & 0xff);
	pret->push_back((id >> 16) & 0xff);
	pret->push_back((id >> 24) & 0xff);
	pret->append(MsgContent);

	return pret;
}

Irole * GameProtocol::GetMsgProcessor(UserDataMsg & _oUserDataMsg)
{
	return m_pRole;
}

/*返回数据发送的通道*/
Ichannel * GameProtocol::GetMsgSender(BytesMsg & _oBytes)
{
	return m_channel;
}
