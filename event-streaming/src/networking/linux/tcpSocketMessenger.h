#pragma once
#include <vector>
#include <string>
#include "../shared/tcpConnectionPool.h"

class TcpSocketMessenger
{
public:
    explicit TcpSocketMessenger(TcpConnectionPool& tcpConnectionPool);
    bool SendRequest(const std::vector<unsigned int>& targetSockets, std::string message);
private:
    TcpConnectionPool& m_TcpConnectionPool;
};
