#pragma once
#include "common.h"
#include "Global.h"

class SnObject 
{
public:
    virtual ~SnObject() {}

    SnObject() 
    {
        _sn = Singleton<Global>::Instance()->GenerateSN();
    }

    SnObject(uint64 sn) 
    {
        _sn = sn;
    }

    uint64 GetSN() const
    {
        return _sn;
    }

protected:
    uint64 _sn;
};