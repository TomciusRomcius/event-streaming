#pragma once
#include "../shared/tcpConnectionPool.h"
#include <functional>
#include <string>
#include "tcpSocketConnectionManager.h"

enum class TcpSocketState
{
    READ_LENGTH,
    READ_BODY
};

struct TcpSocketContext
{
    /// @param so Socket
    /// @param st Socket state
    TcpSocketContext(unsigned int so, TcpSocketState st)
        : socket(so), state(st) {}
    unsigned int socket;
    TcpSocketState state;
    uint32_t bodySize = 0;
};

class TcpMessageReceiver
{
public:
    TcpMessageReceiver(TcpSocketConnectionManager& tcpSocketConnectionManager, TcpConnectionPool& tcpConnectionPool);
    /// @param messageHandler A lambda that takes in a message body and socket
    void TryReceiveMessage(const std::function<void(std::string, unsigned int)>& messageHandler);
private:
    TcpSocketConnectionManager& m_TcpSocketConnectionManager;
    TcpConnectionPool& m_TcpConnectionPool;
    std::unordered_map<unsigned int, uint32_t> m_ProcessingSocketsToMsgSize;
};