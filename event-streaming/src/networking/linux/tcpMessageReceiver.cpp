#include "tcpMessageReceiver.h"
#include <cfloat>
#include <functional>
#include <iostream>
#include <sys/select.h>
#include <sys/socket.h>

TcpMessageReceiver::TcpMessageReceiver(TcpConnectionPool& tcpConnectionPool)
    : m_TcpConnectionPool(tcpConnectionPool)
{

}

void TcpMessageReceiver::TryReceiveMessage(const std::function<void(std::string, unsigned int)>& messageHandler) const
{
    fd_set socketFdSet;
    FD_ZERO(&socketFdSet);

    int maxFd = -1;

    // Add all sockets to the set
    for (const auto& clientSocket : m_TcpConnectionPool)
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

    for (auto& clientSocket : m_TcpConnectionPool)
    {
        if (!FD_ISSET(clientSocket, &socketFdSet))
            continue;
        LOG_DEBUG("Socket {} is ready to read", clientSocket);
        int bufSize = 1024;
        void* buffer = malloc(bufSize);
        ssize_t receivedBytes = recv(clientSocket, buffer, bufSize, 0);
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

        std::string message((char*)buffer, receivedBytes);
        LOG_DEBUG("Received message: '{}'", message);
        free(buffer);
        messageHandler(message, clientSocket); // Call the provided message handler with the received message
    }
}