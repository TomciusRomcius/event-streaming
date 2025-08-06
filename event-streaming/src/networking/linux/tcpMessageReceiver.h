#pragma once
#include "../shared/tcpConnectionPool.h"
#include <functional>
#include <string>

class TcpMessageReceiver
{
public:
    TcpMessageReceiver(TcpConnectionPool& tcpConnectionPool);
    void TryReceiveMessage(const std::function<void(std::string)>& messageHandler) const;
private:
    TcpConnectionPool& m_TcpConnectionPool;
};