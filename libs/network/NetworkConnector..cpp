#include "NetworkConnector.h"

NetworkConnector::NetworkConnector(const std::shared_ptr<Network>& network) :
_network(network)
{
}

bool NetworkConnector::Connect(std::string ip, uint16 port)
{
    _connObj = _network.lock()->CreateConnectObj(::socket(AF_INET, SOCK_STREAM, 0), ip, port);
    _connObj->socket().OpenReuse();
    _connObj->socket().NoBlocking();
    //_connObj->SetReadCallback(std::bind(&NetworkConnector::HandleRead, this));
    _connObj->SetCloseCallback([this](const ConnectObjPtr& conn) {
        _network.lock()->HandleClose(conn);
    });
    if (_connObj->socket().Connect(ip, port))
    {
        _connObj->EnableWrite();
        return true;
    }
    return false;
}

void NetworkConnector::ConnectComplete()
{
    //_network.lock()->CreateConnectObj(_socket.fd(), ip, port);
}

void NetworkConnector::Update()
{
    //TODO 断线重连

    _network.lock()->Update();

    if (!_isConnected)
    {
        if (_connObj->REvents() & EPOLLIN || _connObj->REvents() & EPOLLOUT)        //检测下是否有读写事件，如果有，说明连接成功
        {
            ConnectSuccess();
        }
    }
}

void NetworkConnector::ConnectSuccess()
{
    int optval = -1;
	socklen_t optlen = sizeof(optval);
	int rs = ::getsockopt(_connObj->fd(), SOL_SOCKET, SO_ERROR, (char*)(&optval), &optlen);
	if (rs == 0 && optval == 0)
	{
        _isConnected = true;
        _connObj->DisableWrite();
        LOG_INFO("connect success %s, %d", _connObj->ip().c_str(), _connObj->port());
	}
}

void NetworkConnector::HandleRead()
{
    LOG_INFO("NetworkConnector Read...");
}


