#include <stdexcept>
#include <winsock2.h>
#include "../shared/tcpSocketMessenger.h"
#include "../../application/logging.h"
#include "../../application/utils.h"

constexpr int BATCH_SIZE = 10;

TcpSocketMessenger::TcpSocketMessenger(TcpConnectionPool& tcpConnectionPool)
	: m_TcpConnectionPool(tcpConnectionPool)
{
}

// Used for generating TCP message buffer. Returns a pointer to allocated buffer
std::unique_ptr<void, void(*)(void*)> FormTcpMessage(const std::string& message, uint32_t* bufferSize)
{
	LOG_TRACE("Entered FormTcpMessage");
	uint32_t messageSize = message.length();
	// 4 additional bytes are allocated for a uint32 to specify message size and establish
	// TCP message boundaries
	*bufferSize = 4 + messageSize;
	void* buffer = malloc(*bufferSize);
	uint32_t* sizePointer = reinterpret_cast<uint32_t*>(buffer);
	*sizePointer = HostToBigEndian32(messageSize);
	void* messagePointer = sizePointer + 1;
	memcpy(messagePointer, message.data(), messageSize);
	LOG_DEBUG("TCP message size: {}", *bufferSize);
	LOG_DEBUG("User message size: {}", messageSize);
	return std::unique_ptr<void, void(*)(void*)>(buffer, free);
}

void TcpSocketMessenger::Update()
{
	for (int iteration = 0; iteration < BATCH_SIZE; iteration++)
	{
		if (m_MessageQueue.empty())
			return;
		std::tuple<int, std::string> message = m_MessageQueue.front();
		std::string sMessage = get<1>(message);
		int socket = get<0>(message);
		LOG_DEBUG("Sending a new message to socket {}", socket);
		uint32_t bufferSize;
		std::unique_ptr<void, void (*)(void*)> messageBuffer = FormTcpMessage(sMessage, &bufferSize);
		if (!m_TcpConnectionPool.HasClientSocket(socket))
		{
			LOG_ERROR("Send request failed: socket {} does not exist", socket);
		}

		if (send(socket, (const char*)messageBuffer.get(), bufferSize, 0) == -1)
		{
			LOG_ERROR("Send request failed: '{}'", std::strerror(errno));
		}

		m_MessageQueue.pop();
	}
}

bool TcpSocketMessenger::QueueMessage(const std::vector<SocketType>& targetSockets, std::string message)
{
	LOG_TRACE("Entered TcpSocketMessenger::QueueMessage");
	for (auto socket : targetSockets)
	{
		m_MessageQueue.emplace(std::tuple<int, std::string>(socket, message));
	}
	return true;
}