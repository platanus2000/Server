#pragma once
#include <stdexcept>

template <typename T>
class Singleton
{
public:
    // 禁止外部构造
    Singleton() = delete;
    // 禁止外部析构
    ~Singleton() = delete;
    // 禁止外部复制构造
    Singleton(const Singleton &signal) = delete;
    // 禁止外部赋值操作
    const Singleton &operator=(const Singleton &signal) = delete;
    
    template<typename... Args>
    static T* Instance(Args&&... args)
    {
        static T* _instance = new T(std::forward<Args>(args)...);
        return _instance;
    }
};