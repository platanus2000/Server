#pragma once
#include "common.h"
#include "ThreadObj.h"
#include "SnObject.h"

class Thread : public SnObject
{
public:
    Thread();
    ~Thread();

    void AddThreadObj(const std::shared_ptr<ThreadObj>& obj);

    void Start();
    void Stop();
    void Update();
    bool IsRun() const { return _isRun; }
    
private:
    // 本线程的所有对象    
    std::list<std::shared_ptr<ThreadObj>> _objlist;
    std::list<std::shared_ptr<ThreadObj>> _tmpObjs;
    std::mutex _thread_lock;

    bool _isRun;
    std::thread _thread;
};