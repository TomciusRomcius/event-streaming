#pragma once

#ifdef __linux__
#include "../networking/linux/tcpMessageReceiver.h"
#include "../networking/linux/tcpSocketConnectionManager.h"
#endif

#ifdef WIN32
#include "../networking/windows/tcpMessageReceiver.h"
#include "../networking/windows/tcpSocketConnectionManager.h"
#endif

#include <memory>

#include "../core/memoryPool.h"
#include "../eventSystem/eventSystem.h"
#include "../networking/shared/tcpSocketMessenger.h"
#include "tcpRequestHandlerService.h"

class Application
{
public:
    Application();
    void RegisterRequestStrategies();
    void Start();
private:
    void HandleTcpMessage(const std::string& message, SocketType socket);

    std::unique_ptr<InternalEventBus> m_InternalEventBus;
    std::unique_ptr<TcpConnectionPool> m_TcpConnectionPool;
    std::unique_ptr<TcpSocketConnectionManager> m_TcpConnectionManager;
    std::unique_ptr<TcpSocketMessenger> m_TcpSocketMessenger;
    std::unique_ptr<TcpMessageReceiver> m_TcpMessageReceiver;
    std::unique_ptr<EventSystem> m_EventSystem;
    std::unique_ptr<TcpRequestHandlerService> m_TcpRequestHandlerService;

    std::unique_ptr<MemoryPool> m_MemoryPool;
};
