#pragma once

#ifdef __linux__
#include "../networking/linux/tcpSocketConnectionManager.h"
#include "../networking/linux/tcpMessageReceiver.h"
#endif

#ifdef WIN32
#include "../networking/windows/tcpSocketConnectionManager.h"
#include "../networking/windows/tcpMessageReceiver.h"
#endif

#include <memory>

#include "../eventSystem/eventSystem.h"
#include "tcpRequestHandlerService.h"
#include "../networking/shared/tcpSocketMessenger.h"
#include "../core/memoryPool.h"

class Application
{
public:
	Application();
	void RegisterRequestStrategies();
	void Start();
private:
	void HandleTcpMessage(const std::string& message, unsigned int socket);
	std::unique_ptr<InternalEventBus> m_InternalEventBus;
	std::unique_ptr<TcpConnectionPool> m_TcpConnectionPool;
	std::unique_ptr<TcpSocketConnectionManager> m_TcpConnectionManager;
	std::unique_ptr<TcpSocketMessenger> m_TcpSocketMessenger;
	std::unique_ptr<TcpMessageReceiver> m_TcpMessageReceiver;
	std::unique_ptr<EventSystem> m_EventSystem;
	std::unique_ptr<TcpRequestHandlerService> m_TcpRequestHandlerService;

	std::unique_ptr<MemoryPool> m_MemoryPool;
};
