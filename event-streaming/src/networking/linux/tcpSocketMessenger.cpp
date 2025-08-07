#include "tcpSocketMessenger.h"
#include "../../application/logging.h"
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>

TcpSocketMessenger::TcpSocketMessenger(TcpConnectionPool& tcpConnectionPool)
	: m_TcpConnectionPool(tcpConnectionPool)
{
}

bool TcpSocketMessenger::SendRequest(const std::vector<unsigned int>& targetSockets, std::string message) const
{
	LOG_TRACE("Entered TcpSocketMessenger::SendRequest");
	LOG_DEBUG("Sending message '{}' to {} sockets", message, targetSockets.size());
	
	for (auto socket : targetSockets)
	{
		if (!m_TcpConnectionPool.HasClientSocket(socket))
		{
			LOG_ERROR("Send request failed: socket {} does not exist", socket);
			return false;
		}

		const char* messageBuffer = message.c_str();
		if (send(socket, messageBuffer, message.length(), 0) == -1)
		{
			LOG_ERROR("Send request failed: '{}'", std::strerror(errno));
			return false;
		}
	}
	return true;
}