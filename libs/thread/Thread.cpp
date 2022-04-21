#include "Thread.h"

Thread::Thread() : _isRun(true)
{

}

Thread::~Thread()
{
    auto iter = _objlist.begin();
    while (iter != _objlist.end()) 
    {
        iter = _objlist.erase(iter);
    }

    _objlist.clear();
}

void Thread::AddThreadObj(const std::shared_ptr<ThreadObj>& obj)
{
    obj->RegisterMsgFunction();

    std::lock_guard<std::mutex> guard(_thread_lock);
    _objlist.push_back(obj);
}

void Thread::Start()
{
    _isRun = true;
    _thread = std::thread([this](){
        while(_isRun)
        {
            Update();
        }
    });
}

void Thread::Stop()
{
    if (_isRun)
    {
        _isRun = false;
        if (_thread.joinable()) 
            _thread.join();
    }
}

void Thread::Update()
{
    _thread_lock.lock();
    std::copy(_objlist.begin(), _objlist.end(), std::back_inserter(_tmpObjs));
    _thread_lock.unlock();
    for (auto& pTObj : _tmpObjs) 
    {
        pTObj->Update();
        if (!pTObj->IsActive()) 
        {
            _thread_lock.lock();
            _objlist.remove(pTObj);
            _thread_lock.unlock();
        }
    }
    _tmpObjs.clear();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}