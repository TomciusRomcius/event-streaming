#ifdef __linux

#pragma once
#include <netinet/in.h>
#include "../shared/tcpConnectionPool.h"

class TcpSocketConnectionManager
{
public:
    TcpSocketConnectionManager(InternalEventBus& internalEventBus, TcpConnectionPool& tcpConnectionPool,
                               int serverPort);
    ~TcpSocketConnectionManager();
    void InitializeServerSocket();
    void TryAcceptIncomingConnection();
    void TerminateConnection(unsigned int socket);

private:
    InternalEventBus& m_InternalEventBus;
    TcpConnectionPool& m_TcpConnectionPool;
    unsigned int m_ServerPort;
    unsigned int m_ServerSocket;
    sockaddr_in m_ServerAddress;
};

#endif