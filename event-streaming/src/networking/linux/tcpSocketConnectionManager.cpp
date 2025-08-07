#include "tcpSocketConnectionManager.h"
#include <asm-generic/socket.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

TcpSocketConnectionManager::TcpSocketConnectionManager(TcpConnectionPool& tcpConnectionPool, int serverPort)
    : m_TcpConnectionPool(tcpConnectionPool), m_ServerPort(serverPort)
{
}

TcpSocketConnectionManager::~TcpSocketConnectionManager()
{
	std::cout << "Closing server socket." << '\n';
	close(m_ServerSocket);
}

void TcpSocketConnectionManager::InitializeServerSocket()
{
	std::cout << "Initializing server socket on port " << m_ServerPort << '\n';
	m_ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_ServerSocket == -1)
	{
		std::cerr << std::strerror(errno) << '\n';
		throw std::runtime_error("Failed to create server socket");
	}
	int opt = 1;
	if (setsockopt(m_ServerSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		std::cerr << std::strerror(errno) << '\n';
		throw std::runtime_error("Failed to set socket options");
	}
	m_ServerAddress.sin_family = AF_INET;
	m_ServerAddress.sin_port = htons(m_ServerPort);
	m_ServerAddress.sin_addr.s_addr = INADDR_ANY;

	if (bind(m_ServerSocket, (sockaddr*)&m_ServerAddress, sizeof(m_ServerAddress)) == -1)
	{
		std::cerr << std::strerror(errno) << '\n';
		throw std::runtime_error("Failed to bind server socket");
	}
	if (listen(m_ServerSocket, 5) == -1)
	{
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
		std::cerr << "An error occured while examining socket file descriptor" << '\n';
		return;
	}

	std::cout << "Incoming tcp connection!" << '\n';
	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientSocket = accept(m_ServerSocket, (sockaddr*)&clientAddr, &clientAddrLen);
	if (clientSocket == -1)
	{
		std::cerr << "An error occured while accepting an incoming TCP connection" << '\n';
		return;
	}

	m_TcpConnectionPool.AddClientSocket(clientSocket);
}

void TcpSocketConnectionManager::TerminateConnection()
{
    // TODO: Implement connection termination logic
    std::cerr << "TerminateConnection not implemented yet." << '\n';
}
