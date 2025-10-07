#ifdef WIN32

#include <functional>
#include <iostream>
#include <winsock2.h>
#include "tcpMessageReceiver.h"
#include "../../application/utils.h"

TcpMessageReceiver::TcpMessageReceiver(
	TcpSocketConnectionManager& tcpSocketConnectionManager,
	TcpConnectionPool& tcpConnectionPool)
	: m_TcpSocketConnectionManager(tcpSocketConnectionManager), m_TcpConnectionPool(tcpConnectionPool)
{
}

void TcpMessageReceiver::TryReceiveMessage(const std::function<void(std::string, unsigned int)>& messageHandler)
{
	std::set<SOCKET> clientSockets = m_TcpConnectionPool.GetClientSockets();
	if (clientSockets.empty())
	{
		return;
	}

	FD_SET socketFdSet;
	FD_ZERO(&socketFdSet);

	// Add all sockets to the set
	for (const auto& clientSocket : m_TcpConnectionPool.GetClientSockets())
	{
		FD_SET(clientSocket, &socketFdSet);
	}

	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1000;
	int selectResult = select(0, &socketFdSet, nullptr, nullptr, &timeout);

	if (selectResult == 0)
		return;

	if (selectResult == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		LOG_ERROR(
			"Failed to determine status of sockets: select() failed'{}'",
			error
		);
		return;
	}

	for (auto& clientSocket : clientSockets)
	{
		if (!FD_ISSET(clientSocket, &socketFdSet))
			continue;
		LOG_DEBUG("Socket {} is ready to read", clientSocket);

		if (m_ProcessingSocketsToMsgSize.contains(clientSocket))
		{
			int bufSize = m_ProcessingSocketsToMsgSize.at(clientSocket);
			char* buffer = (char*)malloc(bufSize);
			int receivedBytes = recv(clientSocket, buffer, bufSize, 0);
			if (receivedBytes == 0) // Connection closed
			{
				m_TcpConnectionPool.RemoveClientSocket(clientSocket);
				continue;
			}
			if (receivedBytes < 0)
			{
				LOG_ERROR(
					"Failed to read incoming data for socket {}: '{}'",
					clientSocket,
					std::strerror(errno)
				);
				free(buffer);
				continue; // Skip to the next socket
			}

			if (receivedBytes != bufSize)
			{
				LOG_WARN("Malformed request");
				m_TcpSocketConnectionManager.TerminateConnection(clientSocket);
				continue;
			}
			std::string message((char*)buffer, receivedBytes <= bufSize ? receivedBytes : bufSize);
			LOG_DEBUG("Received message: '{}'", message);
			free(buffer);
			m_ProcessingSocketsToMsgSize.erase(clientSocket);
			messageHandler(message, clientSocket); // Call the provided message handler with the received message
		}

		else
		{
			LOG_DEBUG("Reading request body size");
			int bufSize = 4;
			char* buffer = (char*)malloc(bufSize);
			if (recv(clientSocket, buffer, bufSize, 0) == 0)
			{
				m_TcpConnectionPool.RemoveClientSocket(clientSocket);
				continue;
			}

			uint32_t requestBytes = BigEndianToHost32(*(uint32_t*)buffer);
			LOG_DEBUG("Socket {} message size is {}", clientSocket, requestBytes);
			m_ProcessingSocketsToMsgSize.insert({ clientSocket, requestBytes });
			free(buffer);
		}
	}
}

#endif