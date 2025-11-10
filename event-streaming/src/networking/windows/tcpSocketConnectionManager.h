#pragma once
#include "../../core/internalEventBus.h"
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
	TcpSocketConnectionManager(
		InternalEventBus& internalEventBus, 
		TcpConnectionPool& tcpConnectionPool,
		int serverPort
	);
	~TcpSocketConnectionManager();
	void InitializeServerSocket();
	void TryAcceptIncomingConnection();
	void TerminateConnection(unsigned int socket);
private:
	TcpConnectionPool& m_TcpConnectionPool;
	InternalEventBus& m_InternalEventBus;
	unsigned int m_ServerPort;
	unsigned int m_ServerSocket;
	sockaddr_in m_ServerAddress;
};