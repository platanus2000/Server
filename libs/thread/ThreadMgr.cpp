#include "ThreadMgr.h"

ThreadMgr::~ThreadMgr()
{

}

void ThreadMgr::StartAllThread()
{
    for (auto& th : _threads)
    {
        th.second->Start();
    }
}

bool ThreadMgr::IsGameLoop()
{
    for (auto& th : _threads)
    {
        if (th.second->IsRun())
            return true;
    }
    return false;
}

void ThreadMgr::NewThread()
{
    std::lock_guard<std::mutex> guard(_thread_lock);
    std::shared_ptr<Thread> th = std::make_shared<Thread>();
    _threads[th->GetSN()] = th;
}

void ThreadMgr::AddObjToThread(const std::shared_ptr<ThreadObj>& obj)
{
    std::lock_guard<std::mutex> guard(_thread_lock);

    // 在加入之前初始化一下
    if (!obj->Init())
    {
        std::cout << "AddThreadObj Failed. ThreadObject init failed." << std::endl;
        return;
    }

    // 找到上一次的线程	
    auto iter = _threads.begin();
    if (_lastThreadSn > 0)
    {
        iter = _threads.find(_lastThreadSn);
    }

    if (iter == _threads.end())
    {
        // 没有找到，可能没有配线程
        std::cout << "AddThreadObj Failed. no thead." << std::endl;
        return;
    }

    // 取到它的下一个活动线程
    do
    {
        ++iter;
        if (iter == _threads.end())
            iter = _threads.begin();
    } while (!(iter->second->IsRun()));

    auto pThread = iter->second;
    pThread->AddThreadObj(obj);
    _lastThreadSn = pThread->GetSN();
    //std::cout << "add obj to thread.id:" << pThread->GetSN() << std::endl;
}