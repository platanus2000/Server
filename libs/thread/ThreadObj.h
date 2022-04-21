#pragma once

class ThreadObj
{
public:
    virtual ~ThreadObj() {};
    virtual bool Init( ){ return true; }
    virtual void RegisterMsgFunction( ) {};
    virtual void Update( ) = 0;

    bool IsActive() const { return _active; };
protected:
    bool _active = true;
};