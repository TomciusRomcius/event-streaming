#pragma once
#include "../shared/tcpConnectionPool.h"
#include <functional>
#include <string>

class TcpMessageReceiver
{
public:
    TcpMessageReceiver(TcpConnectionPool& tcpConnectionPool);
    /// @param messageHandler A lambda that takes in a message body and socket
    void TryReceiveMessage(const std::function<void(std::string, unsigned int)>& messageHandler) const;
private:
    TcpConnectionPool& m_TcpConnectionPool;
};