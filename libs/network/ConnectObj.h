#pragma once
#include <memory>
#include <string>
#include "Buffer.h"
#include "Socket.h"
#include "common.h"
#include "SnObject.h"
#include <google/protobuf/message.h>

using google::protobuf::Message;
class Network;
class ConnectObj : public std::enable_shared_from_this<ConnectObj>, public SnObject
{
public:
    typedef std::function<void()> CallBack;
    typedef std::function<void(const ConnectObjPtr&)> CloseCallBack;

    ConnectObj(const std::weak_ptr<Network>& network, SOCKET sockfd, std::string ip = "", uint16 port = 0);
    ~ConnectObj();
    std::unique_ptr<Message> GetMessage();
    SOCKET fd() const { return _socket.fd(); }
    const std::string& ip() const { return _ip; }
    uint16 port() const { return _port; }
    Socket& socket() { return _socket; }

    void HandleEvent();

    bool DefaultRead();
    void DefaultSend();
    void HandleClose();
    bool AddSendData(uint16 msgID, const std::string& strData);
    void AddSendData(const google::protobuf::Message& message);

    int32 Events() const { return _events; }
    int32 REvents() const { return _revents; }
    void SetEvents(int32 events) { _events = events; }
    void SetREvents(int32 revents) { _revents = revents; }
    void EnableRead();
    void DisableRead();
    void EnableWrite();
    void DisableWrite();
    void DisableAll();

    std::shared_ptr<Buffer>& InputBuffer() { return _inputBuffer; }
    std::shared_ptr<Buffer>& OutputBuffer() { return _outputBuffer; }
    void SetMessageCallback(const MessageCallBack& cb) { _messagecallback = cb; }
    void SetReadCallback(const CallBack& cb) { _readcallback = cb; }
    void SetWriteCallback(const CallBack& cb) { _writecallback = cb; }
    void SetCloseCallback(const CloseCallBack& cb) { _closecallback = cb; }
    void SetErrorCallback(const CallBack& cb) { _errorcallback = cb; }

private:
    std::weak_ptr<Network> _network;
    Socket _socket;
    int32 _events = 0;
    int32 _revents = 0;
    std::string _ip;
    uint16_t _port = 0;
    std::shared_ptr<Buffer> _inputBuffer;
    std::shared_ptr<Buffer> _outputBuffer;
    CallBack _readcallback;
    CallBack _writecallback;
    CloseCallBack _closecallback;
    CallBack _errorcallback;
    MessageCallBack _messagecallback;
};