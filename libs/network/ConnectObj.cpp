#include "ConnectObj.h"
#include "common.h"
#include "Logger.h"
#include "Network.h"
#include <sys/unistd.h>
#include <sys/epoll.h>

ConnectObj::ConnectObj(const std::weak_ptr<Network>& network, SOCKET sockfd, std::string ip, uint16 port) : 
_network(network),
_socket(sockfd), 
_events(0),
_revents(0),
_ip(ip),
_port(port),
_inputBuffer(std::make_shared<Buffer>()),
_outputBuffer(std::make_shared<Buffer>()),
_readcallback(std::bind(&ConnectObj::DefaultRead, this)),
_writecallback(std::bind(&ConnectObj::DefaultSend, this)),
_closecallback(std::bind(&ConnectObj::HandleClose, this))
{

}

ConnectObj::~ConnectObj()
{
}

std::unique_ptr<Message> ConnectObj::GetMessage()
{
    return nullptr;
}

void ConnectObj::HandleEvent()
{
    if ((_revents & EPOLLRDHUP) && !(_revents & EPOLLIN))
    {
        HandleClose();
    }
    if (_revents & EPOLLERR)     //出错处理
    {
        if (_errorcallback)
            _errorcallback();
    }
    if (_revents & (EPOLLIN | EPOLLRDHUP | EPOLLPRI))
    {
        if (_readcallback)
            _readcallback();
    }
    if (_revents & EPOLLOUT)
    {
        if (_writecallback)
            _writecallback();
    }
}


bool ConnectObj::DefaultRead()
{
    int savedErrno = 0;
    ssize_t len = _inputBuffer->readFd(_socket.fd(), &savedErrno);
    if (len == 0)
    {
        HandleClose();
        return false;
    }
    else if (len < 0)
    {
        if (savedErrno == EINTR || savedErrno == EWOULDBLOCK || savedErrno == EAGAIN)   //EINTR 系统中断 EAGAIN 非阻塞+无数据可读
			return true;
        return false;
    }
    else
    {
        while (len > 0)
        { 
            //一个消息头
            if (_inputBuffer->readableBytes() < HEAD_SIZE)
                break;
            MsgHeader* poHead = (MsgHeader*)_inputBuffer->peek();
            if (!poHead)
                return false;
            //包是否完整
            poHead->msgID = ntohs(poHead->msgID);
            poHead->dataLen = ntohl(poHead->dataLen);
            if (poHead->dataLen > _inputBuffer->readableBytes() - HEAD_SIZE)
                break;

            //数据处理回调
            _messagecallback(shared_from_this(), _inputBuffer, poHead);
            // char buf[10240];
            // strncpy(buf, _inputBuffer->peek() + HEAD_SIZE, datalen);
            // LOG_INFO("RECV: %s", buf);

            //重置buffer
            _inputBuffer->retrieve(poHead->dataLen + HEAD_SIZE);
            len -= poHead->dataLen + HEAD_SIZE;
        }
    }
    return true;
}

void ConnectObj::DefaultSend()
{
    int32 needSendLen = _outputBuffer->readableBytes();
    while (needSendLen > 0)
    {
        //LOG_DEBUG("needSendLen %d", needSendLen);
        int len = ::send(_socket.fd(), _outputBuffer->beginRead(), _outputBuffer->readableBytes(), 0);
        if (len > 0)
        {
            _outputBuffer->retrieve(len);
            needSendLen -= len;
        }
        if (len == 0)
        {
            LOG_FATAL("send error, len == 0");
        }
        if (len == -1)
        {
            LOG_FATAL("send error, ip: %s, port: %d", _ip.c_str(), _port);
        }
    }
}

void ConnectObj::HandleClose()
{
    DisableAll();
    // TcpConnectionPtr guardThis(shared_from_this());
    // connectionCallback_(guardThis);
    // // must be the last line
    // closeCallback_(guardThis);
    if (_closecallback)
        _closecallback(shared_from_this());
}

void ConnectObj::EnableRead()
{
    _events |= kReadEvent;
    _network.lock()->ModifyEvent(_socket.fd(), _events);
}

void ConnectObj::DisableRead()
{
    _events &= ~kReadEvent;
    _network.lock()->ModifyEvent(_socket.fd(), _events);
}

void ConnectObj::EnableWrite()
{
    _events |= kWriteEvent;
    _network.lock()->ModifyEvent(_socket.fd(), _events);
}

void ConnectObj::DisableWrite()
{
    _events &= ~kWriteEvent;
    _network.lock()->ModifyEvent(_socket.fd(), _events);
}

void ConnectObj::DisableAll()
{
    _events = kNoneEvent;
    _network.lock()->ModifyEvent(_socket.fd(), _events);
}

bool ConnectObj::AddSendData(uint16 msgID, const std::string& data)
{
    if (_outputBuffer->writableBytes() < HEAD_SIZE + data.length())
        return false;
    MsgHeader* header = (MsgHeader*)_outputBuffer->beginWrite();
    header->msgID = htons(msgID);
    header->dataLen = htonl(data.length());
    _outputBuffer->hasWritten(HEAD_SIZE);
    _outputBuffer->append(data.c_str(), data.length());
    return true;
}

void ConnectObj::AddSendData(const google::protobuf::Message& message)
{

}
