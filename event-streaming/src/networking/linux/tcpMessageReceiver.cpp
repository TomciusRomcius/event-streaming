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

void TcpMessageReceiver::TryReceiveMessage(std::function<void(std::string)> messageHandler)
{
    std::cout << "TryReceiveMessage" << '\n';
    
    fd_set socketFdSet;
    FD_ZERO(&socketFdSet);

    unsigned int maxFd = m_TcpConnectionPool.GetHighestSocketDescriptor();
    if (maxFd < 0)
    {
        return; // No valid sockets to monitor
    }

    int selectResult = select(maxFd + 1, &socketFdSet, nullptr, nullptr, nullptr);
    for (auto& clientSocket : m_TcpConnectionPool)
    {
        if (!FD_ISSET(clientSocket, &socketFdSet))
            continue;
        std::cout << "Checking socket: " << clientSocket << '\n';

        if (selectResult < 0) // Error
        {
            std::cerr << "Error in select for socket: " << clientSocket << '\n';
            continue; // Skip to the next socket
        }
        else if (selectResult > 0) // Ready to read
        {
            std::cout << "Socket " << clientSocket << " is ready to read.\n";
            int bufSize = 1024;
            void* buffer = malloc(bufSize);
            int receivedBytes = recv(clientSocket, buffer, bufSize, 0);
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
}