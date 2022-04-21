#pragma once
#include "common.h"
#include "Thread.h"
#include "ThreadObj.h"

class ThreadMgr
{
public:
    ~ThreadMgr();
    void StartAllThread();
    bool IsGameLoop();
    void NewThread();
    void AddObjToThread(const std::shared_ptr<ThreadObj>& obj);

private:
    uint64 _lastThreadSn { 0 }; // 实现线程对象均分

    std::mutex _thread_lock;
    std::map<uint64, std::shared_ptr<Thread>> _threads;
};