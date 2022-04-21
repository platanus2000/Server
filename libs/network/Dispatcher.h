#pragma once
#include "common.h"
#include <google/protobuf/message.h>

typedef std::shared_ptr<google::protobuf::Message> MessagePtr;

class Callback
{
public:
    virtual ~Callback() = default;
    virtual void onMessage(const ConnectObjPtr&, const MessagePtr& message) const = 0;
};

template <typename T>
class CallbackT : public Callback
{
    static_assert(std::is_base_of<google::protobuf::Message, T>::value,
                "T must be derived from gpb::Message.");
public:
    typedef std::function<void (const muduo::net::TcpConnectionPtr&,
                                const std::shared_ptr<T>& message)> ProtobufMessageTCallback;

    CallbackT(const ProtobufMessageTCallback& callback)
    : callback_(callback)
    {
    }

    void onMessage(const ConnectObjPtr& conn, const MessagePtr& message) const override
    {
        std::shared_ptr<T> concrete = down_pointer_cast<T>(message);        //指针的下行转换，这是一个疑问点
        assert(concrete != NULL);
        callback_(conn, concrete);
    }

private:
    ProtobufMessageTCallback callback_;
};

class ProtobufDispatcher
{
public:
    typedef std::function<void (const ConnectObjPtr&,
                                const MessagePtr& message)> ProtobufMessageCallback;

    explicit ProtobufDispatcher(const ProtobufMessageCallback& defaultCb) : defaultCallback_(defaultCb)
    {
    }

    void onProtobufMessage(const ConnectObjPtr& conn,
                            const MessagePtr& message) const
    {
        CallbackMap::const_iterator it = callbacks_.find(message->GetDescriptor());
        if (it != callbacks_.end())
        {
            it->second->onMessage(conn, message);
        }
        else
        {
            defaultCallback_(conn, message);
        }
    }

    template<typename T>
    void registerMessageCallback(const typename CallbackT<T>::ProtobufMessageTCallback& callback)
    {
        std::shared_ptr<CallbackT<T> > pd(new CallbackT<T>(callback));
        callbacks_[T::descriptor()] = pd;
    }

private:
    typedef std::map<const google::protobuf::Descriptor*, std::shared_ptr<Callback>> CallbackMap;
    CallbackMap callbacks_;
    ProtobufMessageCallback defaultCallback_;
};