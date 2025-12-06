#ifdef __linux__

#pragma once
#include <functional>
#include <string>
#include "../../core/memoryPool.h"
#include "../shared/tcpConnectionPool.h"
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
    TcpSocketContext(unsigned int so, TcpSocketState st) : socket(so), state(st) {}

    unsigned int socket;
    TcpSocketState state;
    uint32_t bodySize = 0;
};

class TcpMessageReceiver
{
public:
    inline TcpMessageReceiver(TcpSocketConnectionManager& tcpSocketConnectionManager,
                              TcpConnectionPool& tcpConnectionPool, MemoryPool& memoryPool,
                              std::function<void(std::string, SocketType)>&& messageHandler) :
        m_TcpSocketConnectionManager(tcpSocketConnectionManager), m_TcpConnectionPool(tcpConnectionPool),
        m_MemoryPool(memoryPool), m_MessageHandler(std::move(messageHandler))
    {}
    void TryReceiveMessage();
private:
    TcpSocketConnectionManager& m_TcpSocketConnectionManager;
    TcpConnectionPool& m_TcpConnectionPool;
    std::function<void(std::string, unsigned int)> m_MessageHandler;
    MemoryPool& m_MemoryPool;
    std::unordered_map<unsigned int, uint32_t> m_ProcessingSocketsToMsgSize;
};

#endif
