#ifdef __linux__

#include "tcpMessageReceiver.h"
#include <cfloat>
#include <functional>
#include <iostream>
#include <sys/select.h>
#include <sys/socket.h>
#include "../../application/utils.h"
#include <sys/ioctl.h>

TcpMessageReceiver::TcpMessageReceiver(
	TcpSocketConnectionManager& tcpSocketConnectionManager,
	TcpConnectionPool& tcpConnectionPool)
	: m_TcpSocketConnectionManager(tcpSocketConnectionManager), m_TcpConnectionPool(tcpConnectionPool)
{
}

void TcpMessageReceiver::TryReceiveMessage(const std::function<void(std::string, unsigned int)>& messageHandler)
{
	fd_set socketFdSet;
	FD_ZERO(&socketFdSet);

	int maxFd = -1;

	// Add all sockets to the set
	for (const auto& clientSocket : m_TcpConnectionPool.GetClientSockets())
	{
		FD_SET(clientSocket, &socketFdSet);
		if ((int)clientSocket > maxFd)
		{
			maxFd = clientSocket;
		}
	}

	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 10000;
	int selectResult = select(maxFd + 1, &socketFdSet, nullptr, nullptr, &timeout);

	if (selectResult == 0)
		return;

	if (selectResult == -1)
	{
		LOG_ERROR(
			"Failed to determine status of sockets: select() failed'{}'",
			std::strerror(errno)
		);
		return;
	}

	for (auto& clientSocket : m_TcpConnectionPool.GetClientSockets())
	{
		if (!FD_ISSET(clientSocket, &socketFdSet))
			continue;
		LOG_DEBUG("Socket {} is ready to read", clientSocket);

		if (m_ProcessingSocketsToMsgSize.contains(clientSocket))
		{
			int bufSize = m_ProcessingSocketsToMsgSize.at(clientSocket);
			void* buffer = malloc(bufSize);
			ssize_t receivedBytes = recv(clientSocket, buffer, bufSize, 0);
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
			void* buffer = malloc(bufSize);
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