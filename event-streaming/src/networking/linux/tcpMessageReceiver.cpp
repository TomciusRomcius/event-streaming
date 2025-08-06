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

void TcpMessageReceiver::TryReceiveMessage(const std::function<void(std::string)>& messageHandler) const
{
    std::cout << "TryReceiveMessage" << '\n';
    
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
    
    if (selectResult <= 0)
    {
        return;
    }

    for (auto& clientSocket : m_TcpConnectionPool)
    {
        if (!FD_ISSET(clientSocket, &socketFdSet))
            continue;
        std::cout << "Checking socket: " << clientSocket << '\n';
        std::cout << "Socket " << clientSocket << " is ready to read.\n";
        int bufSize = 1024;
        void* buffer = malloc(bufSize);
        ssize_t receivedBytes = recv(clientSocket, buffer, bufSize, 0);
        if (receivedBytes < 0)
        {
            std::cerr << "Error receiving message from socket: " << clientSocket << '\n';
            free(buffer);
            continue; // Skip to the next socket
        }

        std::string message((char*)buffer, receivedBytes);
        free(buffer);
        messageHandler(message); // Call the provided message handler with the received message
    }
}