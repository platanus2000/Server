#pragma once
#include "zinx.h"

class GameChannel;
class GameRole;
class GameProtocol : public Iprotocol
{
public:
    GameChannel *m_channel = NULL;
    GameProtocol();
    virtual ~GameProtocol();

    // 通过 Iprotocol 继承
    virtual UserData *raw2request(std::string _szInput) override;
    virtual std::string *response2raw(UserData &_oUserData) override;
    virtual Irole *GetMsgProcessor(UserDataMsg &_oUserDataMsg) override;
    virtual Ichannel *GetMsgSender(BytesMsg &_oBytes) override;

    GameRole* m_pRole = nullptr;
private:
    std::string szLast;
};