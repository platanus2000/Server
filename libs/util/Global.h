#pragma once
#include "Singleton.h"
#include "common.h"
#include "Timeutil.h"

class Global
{
public:
	uint64 GenerateSN()	// SN = 64位,时间+服务器ID+ticket
    {
        std::lock_guard<std::mutex> guard(_mtx);
        uint64 ret = (TimeTick << 32) + (_serverId << 16) + _snTicket;
        _snTicket += 1;
        return ret;
    }

	int32 YearDay;
	timeutil::Time TimeTick;

private:
	std::mutex _mtx;
	uint32 _snTicket = 1;
	uint32 _serverId = 0;
};