#pragma once
#include <vector>
#include <string>
#include <queue>
#include "../shared/tcpConnectionPool.h"
#include "../shared/tcpRequest.h"

class TcpSocketMessenger
{
public:
	explicit TcpSocketMessenger(TcpConnectionPool &tcpConnectionPool);
	void Update();
	bool QueueMessage(const std::vector<SocketType> &targetSockets, std::string message);

private:
	TcpConnectionPool &m_TcpConnectionPool;
	// Holds a tuple(socket, message)
	std::queue<std::tuple<SocketType, std::string>> m_MessageQueue;
};
