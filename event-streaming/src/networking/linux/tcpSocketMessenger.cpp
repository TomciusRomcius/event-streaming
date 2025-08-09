#include "tcpSocketMessenger.h"
#include "../../application/logging.h"
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include "../../application/utils.h"

TcpSocketMessenger::TcpSocketMessenger(TcpConnectionPool& tcpConnectionPool)
	: m_TcpConnectionPool(tcpConnectionPool)
{
}

// Used for generating TCP message buffer. Returns a pointer to allocated buffer
void* FormTcpMessage(const std::string& message, uint32_t* bufferSize)
{
	LOG_TRACE("Entered FormTcpMessage");
	uint32_t messageSize = message.length();
	// 4 additional bytes are allocated for a uint32 to specify message size and establish
	// TCP message boundaries
	*bufferSize = 4 + messageSize;
	void* buffer = malloc(*bufferSize);
	uint32_t* sizePointer = reinterpret_cast<uint32_t*>(buffer);
	*sizePointer = HostToBigEndian32(messageSize);
	void* messagePointer = reinterpret_cast<void*>(sizePointer + 1);
	memcpy(messagePointer, message.c_str(), messageSize);
	LOG_DEBUG("TCP message size: {}", *bufferSize);
	LOG_DEBUG("User message size: {}", messageSize);
	return buffer;
}

bool TcpSocketMessenger::SendRequest(const std::vector<unsigned int>& targetSockets, std::string message) const
{
	LOG_TRACE("Entered TcpSocketMessenger::SendRequest");
	LOG_DEBUG("Sending message '{}' to {} sockets", message, targetSockets.size());

	uint32_t bufferSize;
	void* messageBuffer = FormTcpMessage(message, &bufferSize);

	for (auto socket : targetSockets)
	{
		if (!m_TcpConnectionPool.HasClientSocket(socket))
		{
			LOG_ERROR("Send request failed: socket {} does not exist", socket);
			return false;
		}

		if (send(socket, messageBuffer, bufferSize, 0) == -1)
		{
			LOG_ERROR("Send request failed: '{}'", std::strerror(errno));
			return false;
		}
	}

	free(messageBuffer);
	return true;
}