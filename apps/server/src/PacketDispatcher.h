#pragma once
#include "common.h"

struct NetPacket
{
    NetPacket() : msgID(0), length(0) {}
    NetPacket(int16 _msgID, int32 _length, const char* _buff) : msgID(_msgID), length(_length), buff(_buff) {}
    NetPacket& operator=(NetPacket& tmp)
    {
        msgID = tmp.msgID;
        length = tmp.length;
        buff = std::move(tmp.buff);
        tmp.msgID = 0;
        return *this;
    }
    int16 msgID = 0;
    int32 length = 0;
    std::string buff;   //TODO 改成内存池
};

class IPacketDispatcher
{
public:
    typedef std::function<void(NetPacket&)> Handler;
    virtual void RegisterHandler(uint16 msgID, Handler handler) = 0;
	virtual bool DispatchPacket(NetPacket& pNetPacket) = 0;
	// virtual bool OnCloseConnect(uint32 nConnID) = 0;
	// virtual bool OnNewConnect(uint32 nConnID) = 0;
	// virtual bool OnSecondTimer() = 0;
};