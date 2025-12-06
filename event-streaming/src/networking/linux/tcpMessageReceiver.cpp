#ifdef __linux__

#include "tcpMessageReceiver.h"
#include <cfloat>
#include <functional>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include "../../application/utils.h"

void TcpMessageReceiver::TryReceiveMessage()
{
    fd_set socketFdSet;
    FD_ZERO(&socketFdSet);

    int maxFd = -1;

    // Add all sockets to the set
    for (const auto& clientSocket: m_TcpConnectionPool.GetClientSockets())
    {
        FD_SET(clientSocket, &socketFdSet);
        if ((int) clientSocket > maxFd)
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
        LOG_ERROR("Failed to determine status of sockets: select() failed'{}'", std::strerror(errno));
        return;
    }

    for (auto& clientSocket: m_TcpConnectionPool.GetClientSockets())
    {
        if (!FD_ISSET(clientSocket, &socketFdSet))
            continue;
        LOG_DEBUG("Socket {} is ready to read", clientSocket);

        if (m_ProcessingSocketsToMsgSize.contains(clientSocket))
        {
            int bufSize = m_ProcessingSocketsToMsgSize.at(clientSocket);
            std::optional<MemoryChunkUser> memoryChunkUser = m_MemoryPool.GetMemoryChunk(bufSize);
            assert(memoryChunkUser.has_value());
            void* buffer = memoryChunkUser.value().GetBuffer();
            ssize_t receivedBytes = recv(clientSocket, buffer, bufSize, 0);
            if (receivedBytes == 0) // Connection closed
            {
                m_TcpSocketConnectionManager.TerminateConnection(clientSocket);
                m_ProcessingSocketsToMsgSize.erase(clientSocket);
                continue;
            }
            if (receivedBytes < 0)
            {
                LOG_ERROR("Failed to read incoming data for socket {}: '{}'", clientSocket, std::strerror(errno));
                m_TcpSocketConnectionManager.TerminateConnection(clientSocket);
                m_ProcessingSocketsToMsgSize.erase(clientSocket);
                free(buffer);
                continue; // Skip to the next socket
            }

            if (receivedBytes != bufSize)
            {
                LOG_WARN("Malformed request");
                m_TcpSocketConnectionManager.TerminateConnection(clientSocket);
                m_ProcessingSocketsToMsgSize.erase(clientSocket);
                continue;
            }
            std::string message((char*) buffer, receivedBytes <= bufSize ? receivedBytes : bufSize);
            LOG_DEBUG("Received message: '{}'", message);
            m_ProcessingSocketsToMsgSize.erase(clientSocket);
            // Call the provided message handler with
            // the received message
            m_MessageHandler(std::move(message), clientSocket);
        } else
        {
            LOG_DEBUG("Reading request body size");
            int bufSize = 4;
            void* buffer = malloc(bufSize);
            if (recv(clientSocket, buffer, bufSize, 0) == 0)
            {
                m_TcpSocketConnectionManager.TerminateConnection(clientSocket);
                continue;
            }

            uint32_t requestBytes = BigEndianToHost32(*(uint32_t*) buffer);
            LOG_DEBUG("Socket {} message size is {}", clientSocket, requestBytes);
            m_ProcessingSocketsToMsgSize.insert({clientSocket, requestBytes});
        }
    }
}

#endif
