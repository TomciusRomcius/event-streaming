#pragma once
#include <vector>
#include <string>
#include <queue>
#include "../shared/tcpConnectionPool.h"
#include "../shared/tcpRequest.h"
#include "../../core/memoryPool.h"

class TcpSocketMessenger
{
public:
	explicit TcpSocketMessenger(TcpConnectionPool& tcpConnectionPool, MemoryPool& memoryPool);
	void Update();
	bool QueueMessage(const std::vector<SocketType>& targetSockets, std::string message);
private:
	std::unique_ptr<void, void(*)(void*)> FormTcpMessage(const std::string& message, uint32_t* bufferSize);
	TcpConnectionPool& m_TcpConnectionPool;
	MemoryPool& m_MemoryPool;
	// Holds a tuple(socket, message)
	std::queue<std::tuple<SocketType, std::string>> m_MessageQueue;
};
