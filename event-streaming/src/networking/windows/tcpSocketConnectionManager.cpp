#ifdef WIN32

#include <cstring>
#include <iostream>
#include <winsock2.h>
#include "tcpSocketConnectionManager.h"
#include "../../application/logging.h"
#include "../../core/internalEventTypes.h"

TcpSocketConnectionManager::TcpSocketConnectionManager(InternalEventBus& internalEventBus,
	TcpConnectionPool& tcpConnectionPool,
	int serverPort)
	: m_InternalEventBus(internalEventBus), m_TcpConnectionPool(tcpConnectionPool), m_ServerPort(serverPort)
{
}

TcpSocketConnectionManager::~TcpSocketConnectionManager()
{
	std::cout << "Closing server socket." << '\n';
	closesocket(m_ServerSocket);
}

void TcpSocketConnectionManager::InitializeServerSocket()
{
	LOG_INFO("Initializing WSA");
	WORD version = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(version, &wsaData) == -1)
	{
		int error = WSAGetLastError();
		LOG_ERROR("Failed to initialize WSA: '{}'", error);
		throw std::runtime_error("Failed to initialize WSA");
	}
	LOG_INFO("Initializing server socket on port: {}", m_ServerPort);
	m_ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_ServerSocket == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		LOG_ERROR("Failed to setup server socket: '{}'", error);
		throw std::runtime_error("Failed to create server socket");
	}

	BOOL opt = TRUE;
	if (setsockopt(m_ServerSocket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char*)&opt, sizeof(opt)) == SOCKET_ERROR)
	{
		LOG_ERROR("Failed to setup server socket options: '{}'", std::strerror(errno));
		throw std::runtime_error("Failed to set socket options");
	}
	m_ServerAddress.sin_family = AF_INET;
	m_ServerAddress.sin_port = htons(m_ServerPort);
	m_ServerAddress.sin_addr.s_addr = INADDR_ANY;

	if (bind(m_ServerSocket, (sockaddr*)&m_ServerAddress, sizeof(m_ServerAddress)) == -1)
	{
		LOG_ERROR("Failed to bind server socket to server address: '{}'", std::strerror(errno));
		throw std::runtime_error("Failed to bind server socket");
	}

	if (listen(m_ServerSocket, 5) == -1)
	{
		LOG_ERROR("Failed to setup server socket listener: '{}'", std::strerror(errno));
		throw std::runtime_error("Failed to setup server socket listener");
	}
}

void TcpSocketConnectionManager::TryAcceptIncomingConnection()
{
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1000;

	fd_set fdSet;
	FD_ZERO(&fdSet);
	FD_SET(m_ServerSocket, &fdSet);

	int selectResult = select(m_ServerSocket + 1, &fdSet, nullptr, nullptr, &timeout);

	if (selectResult == 0)
	{
		return; // No activity
	}

	if (selectResult == -1)
	{
		LOG_ERROR("An error occured while examining socket file descriptor: '{}'", std::strerror(errno));
		return;
	}

	LOG_DEBUG("Incoming tcp connection");
	sockaddr_in clientAddr;
	int clientAddrLen = sizeof(clientAddr);
	int clientSocket = accept(m_ServerSocket, (sockaddr*)&clientAddr, &clientAddrLen);
	if (clientSocket == -1)
	{
		LOG_ERROR("An error occured while accepting an incoming TCP connection: {}", std::strerror(errno));
		return;
	}
	
	m_TcpConnectionPool.AddClientSocket(clientSocket);
	m_InternalEventBus.ProduceEvent(new ClientConnectedEvent(clientSocket));
}

void TcpSocketConnectionManager::TerminateConnection(unsigned int socket)
{
	SPDLOG_TRACE("Entered TcpSocketConnectionManager::TerminateConnection");
	SPDLOG_DEBUG("Terminating connection for socket {}", socket);
	closesocket(socket);
	m_TcpConnectionPool.RemoveClientSocket(socket);
	m_InternalEventBus.ProduceEvent(new ClientDisconnectedEvent(socket));
}
#endif