#pragma once
#include <deque>
#include "common.h"

class MessageQueue
{
public:
    MessageQueue() {};
    void Push(NetPacket&& packet) 
    { 
        std::lock_guard<std::mutex> lock(mtx);
        _messageQueue.emplace_back(packet); 
    }
    NetPacket Front(NetPacket&& packet) 
    {
        std::lock_guard<std::mutex> lock(mtx);
        return _messageQueue.front();
    }
    void Pop() 
    {
        std::lock_guard<std::mutex> lock(mtx);
        return _messageQueue.pop_front();
    }

    void Swap(MessageQueue& q1, MessageQueue& q2)
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::swap(q1._messageQueue, q2._messageQueue);
    }

private:
    std::deque<NetPacket> _messageQueue;
    std::mutex mtx;
};