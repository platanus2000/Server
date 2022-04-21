#include "Network.h"
#include "ConnectObj.h"

Network::Network() : 
        _epfd(epoll_create(MAX_CLIENT))
{
}

Network::~Network()
{
}

void Network::AddEvent(int sockfd, int flag)
{
    struct epoll_event event;
    event.events = flag;
    event.data.fd = sockfd;
    epoll_ctl(_epfd, EPOLL_CTL_ADD, sockfd, &event);
}

void Network::ModifyEvent(int sockfd, int flag)
{
    struct epoll_event event;
    event.events = flag;
    event.data.fd = sockfd;
    epoll_ctl(_epfd, EPOLL_CTL_MOD, sockfd, &event);
}

void Network::DelEvent(int sockfd)
{
    struct epoll_event event;
    event.events = 0;
    event.data.fd = sockfd;
    epoll_ctl(_epfd, EPOLL_CTL_DEL, sockfd, &event);
}

void Network::HandleClose(const ConnectObjPtr& ptr)
{
    DelEvent(ptr->fd());
    _connectMgr.erase(ptr->fd());
}

void Network::Update()
{
    int nfds = epoll_wait(_epfd, _events, MAX_EVENT, 0);
    if (nfds > 0)
    {
        for (int i = 0; i < nfds; i++)
        {
            SOCKET fd = _events[i].data.fd;
            auto itrFind = _connectMgr.find(fd);
            assert(itrFind != _connectMgr.end());
            auto& conn = itrFind->second;
            assert(conn != nullptr);
            conn->SetREvents(_events[i].events);
            //handleevent
            conn->HandleEvent();

            // if (_events[i].events & EPOLLHUP || _events[i].events & EPOLLRDHUP || _events[i].events & EPOLLERR)     //出错处理
            // {
            //     //错误处理
            //     DelEvent(fd);
            //     _connectMgr.erase(itrFind);                
            // }
            // else if (_events[i].events & EPOLLIN)
            // {
            //     //TODO 改成回调
            //     if (fd == _listener->fd())
            //     {
            //         Accept();
            //     }
            //     else
            //     {
            //         //读取
            //         if (!conn->Recv())
            //         {
            //             //销毁       
            //             DelEvent(fd);
            //             _connectMgr.erase(itrFind);
            //         }
            //     }
            // }
            // else if (_events[i].events & EPOLLOUT)
            // {
            //     //发送
            //     conn->Send();
            //     //发完之后要关闭 EPOLLOUT
            //     conn->DisableWrite();
            // }
        }
    }
    return;
}

std::shared_ptr<ConnectObj>& Network::CreateConnectObj(SOCKET socket, std::string ip, uint16 port)
{
    assert(_connectMgr.find(socket) == _connectMgr.end());
    std::shared_ptr<ConnectObj> conn = std::make_shared<ConnectObj>(shared_from_this(), socket, ip, port);
    conn->SetEvents(kReadEvent);
	AddEvent(socket, kReadEvent);
	_connectMgr.insert(std::make_pair(socket, conn));
    return _connectMgr[socket];
}

void Network::Send(int sockfd, uint16 msgID, const std::string& data)
{
    auto itrFind = _connectMgr.find(sockfd);
    assert(itrFind !=_connectMgr.end());
    auto& conn = itrFind->second;
    assert(conn != nullptr);
    conn->AddSendData(msgID, data);
    conn->EnableWrite();
}

void Network::SendAll(uint16 msgID, const std::string& data)
{
    for (const auto& connRef : _connectMgr)
    {
        auto& conn = connRef.second;
        assert(conn != nullptr);
        conn->AddSendData(msgID, data);
        conn->EnableWrite();
    }
}

