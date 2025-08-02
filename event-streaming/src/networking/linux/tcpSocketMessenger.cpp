#include "tcpSocketMessenger.h"
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>

TcpSocketMessenger::TcpSocketMessenger(TcpConnectionPool& tcpConnectionPool)
	: m_TcpConnectionPool(tcpConnectionPool)
{
}

bool TcpSocketMessenger::SendRequest(std::vector<unsigned int>& targetSockets, std::string message)
{
	for (auto socket : targetSockets)
	{
		if (!m_TcpConnectionPool.HasClientSocket(socket))
		{
			throw std::runtime_error("Trying to send a message to a socket that does not exist!");
		}

		const char* messageBuffer = message.c_str();
		if (send(socket, messageBuffer, message.length(), 0) == -1)
		{
			std::cerr << "Encountered an error while sending a message to a socket" << '\n';
			return false;
		}
	}
	return true;
}