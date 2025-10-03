#pragma once
#include "../shared/tcpConnectionPool.h"
#ifdef __linux__
#include <sys/socket.h>
#endif
#ifdef WIN32
#include <winsock.h>
#endif

class TcpSocketConnectionManager
{
public:
	TcpSocketConnectionManager(TcpConnectionPool& tcpConnectionPool, int serverPort);
	~TcpSocketConnectionManager();
	void InitializeServerSocket();
	void TryAcceptIncomingConnection();
	void TerminateConnection(unsigned int socket);
private:
	TcpConnectionPool& m_TcpConnectionPool;
	unsigned int m_ServerPort;
	unsigned int m_ServerSocket;
	sockaddr_in m_ServerAddress;
};