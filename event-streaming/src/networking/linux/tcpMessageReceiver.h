#pragma once
#include "../shared/tcpConnectionPool.h"
#include <functional>
#include <string>

class TcpMessageReceiver
{
public:
    TcpMessageReceiver(TcpConnectionPool& tcpConnectionPool);
    void TryReceiveMessage(std::function<void(std::string)> messageHandler);
private:
    TcpConnectionPool& m_TcpConnectionPool;
};