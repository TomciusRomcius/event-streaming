#pragma once
#include "../shared/tcpConnectionPool.h"
#include <netinet/in.h>

class TcpSocketConnectionManager
{
public:
    TcpSocketConnectionManager(TcpConnectionPool& tcpConnectionPool, int serverPort);
    ~TcpSocketConnectionManager();
    void InitializeServerSocket();
    void TryAcceptIncomingConnection();
    void TerminateConnection();
private:
    TcpConnectionPool& m_TcpConnectionPool;
    unsigned int m_ServerPort;
    unsigned int m_ServerSocket;
    sockaddr_in m_ServerAddress;
};