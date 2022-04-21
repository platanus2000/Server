#pragma once
#include "common.h"
#include <atomic>

template <typename T>
class CASQueue
{
private:
    struct Node
    {
        Node(T _val) : val(_val), next(nullptr) { }
        T val;
        std::atomic<Node*> next = nullptr;
    };
public:
    CASQueue()
    {
        //初始一个空的头节点
        T EmptyVal;
        pHead = pTail = new Node(EmptyVal);
    }

    CASQueue(CASQueue& tmp)
    {
        
    }

    bool Empty() const { return !count; }

    int32 Size() const { return count; }

    void Push(T&& val)
    {
        Node* const new_node = new Node(val);
        Node* t = pTail;
        Node* old_t = pTail;
        Node* excepted = nullptr;
        do
        {
            while (t->next != nullptr)
            {
                t = t->next;
            }
        } while (!t->next.compare_exchange_weak(excepted, new_node));
        //将最后的节点_tail更换为new_node
        pTail.compare_exchange_weak(old_t, new_node);
        count++;
    }

    // bool Pop(T& val)
    // {
    //     Node* p = nullptr;
    //     do
    //     {
    //         p = pHead; // 头节点，不真正存储数据
    //         if (p->next == nullptr) // 首元节点为空，则返回
    //         {
    //             return false;
    //         }
    //     } while(!pHead.compare_exchange_weak(p, p->next));
    //     val = p->val;
    //     delete p;
    //     p = nullptr;
    //     count--;
    //     return true;
    // }
    bool TryPop(T& val)
    {
        Node* p = nullptr;
        do
        {
            p = pHead.load()->next; // 头节点，不真正存储数据
            if (p == nullptr) // 首元节点为空，则返回
            {
                return false;
            }
        } while(!pHead.load()->next.compare_exchange_weak(p, p->next));
        // if (--count == 0)
        // {
        //     while(!pTail.compare_exchange_weak(p, pHead));
        // }
        pTail.compare_exchange_weak(p, pHead);
        val = p->val;
        delete p;
        p = nullptr;
        return true;
    }
private:
    std::atomic<int32> count = 0;
    std::atomic<Node*> pHead = nullptr;
    std::atomic<Node*>  pTail = nullptr;
};