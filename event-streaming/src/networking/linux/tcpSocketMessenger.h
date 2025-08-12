#pragma once
#include <vector>
#include <string>
#include <queue>
#include "../shared/tcpConnectionPool.h"
#include "../shared/tcpRequest.h"

class TcpSocketMessenger
{
public:
    explicit TcpSocketMessenger(TcpConnectionPool& tcpConnectionPool);
    void Update();
    bool QueueMessage(const std::vector<unsigned int>& targetSockets, std::string message);
private:
    TcpConnectionPool& m_TcpConnectionPool;
    // Holds a tuple(socket, message)
    std::queue<std::tuple<int, std::string>> m_MessageQueue;
};
