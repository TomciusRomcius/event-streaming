#ifdef __linux__

#include "../shared/tcpSocketMessenger.h"
#include <stdexcept>
#include <sys/socket.h>
#include "../../application/logging.h"
#include "networking/shared/utils.h"

constexpr int BATCH_SIZE = 10;

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
        MemoryChunkUser memoryChunkUser = FormTcpMessage(m_MemoryPool, sMessage, &bufferSize);
        auto messageBuffer = static_cast<const char*>(memoryChunkUser.GetBuffer());
        if (!m_TcpConnectionPool.HasClientSocket(socket))
        {
            LOG_ERROR("Send request failed: socket {} does not exist", socket);
        }

        if (send(socket, messageBuffer, bufferSize, 0) == -1)
        {
            LOG_ERROR("Send request failed: '{}'", std::strerror(errno));
        }

        m_MessageQueue.pop();
    }
}

bool TcpSocketMessenger::QueueMessage(const std::vector<SocketType>& targetSockets, std::string message)
{
    LOG_TRACE("Entered TcpSocketMessenger::QueueMessage");
    for (auto socket: targetSockets)
    {
        m_MessageQueue.emplace(socket, message);
    }
    return true;
}

#endif
