#pragma once
#include <queue>
#include <string>
#include <vector>
#include "../../core/memoryPool.h"
#include "../shared/tcpConnectionPool.h"

class TcpSocketMessenger
{
public:
    explicit TcpSocketMessenger(TcpConnectionPool& tcpConnectionPool, MemoryPool& memoryPool);
    void Update();
    bool QueueMessage(const std::vector<SocketType>& targetSockets, std::string message);
private:
    TcpConnectionPool& m_TcpConnectionPool;
    MemoryPool& m_MemoryPool;
    // Holds a tuple(socket, message)
    std::queue<std::tuple<SocketType, std::string>> m_MessageQueue;
};
