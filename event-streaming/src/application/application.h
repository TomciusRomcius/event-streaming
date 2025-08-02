#pragma once

#include <memory>
#include <string>
#include <unistd.h>
#include <iostream>
#include "../networking/linux/tcpSocketConnectionManager.h"
#include "../networking/linux/tcpSocketMessenger.h"
#include "../networking/linux/tcpMessageReceiver.h"
#include "../networking/shared/tcpRequest.h"
#include "../eventSystem/eventSystem.h"

// class TcpRequestParser
// {
// public:
// 	ITcpRequest* ParseRequest(const std::string& request)
// 	{
// 		// Dummy implementation for parsing requests
// 		// In a real application, this would parse the request string and return the appropriate ITcpRequest derived object
// 		if (request == "CREATE_EVENT_TYPE")
// 		{
// 			request
// 			return new CreateEventTypeRequest();
// 		}
// 		else if (request == "SUBSCRIBE_TO_EVENT")
// 		{
// 			return new SubscribeToEventRequest();
// 		}
// 		return nullptr;
// 	}
// };

class Application
{
public:
	Application()
	{
		unsigned int serverPort = 9000;
		m_EventSystem = std::make_unique<EventSystem>();
		m_TcpConnectionPool = std::make_unique<TcpConnectionPool>();
		m_TcpConnectionManager = std::make_unique<TcpSocketConnectionManager>(*m_TcpConnectionPool, serverPort);
		m_TcpSocketMessenger = std::make_unique<TcpSocketMessenger>(*m_TcpConnectionPool);
		m_TcpMessageReceiver = std::make_unique<TcpMessageReceiver>(*m_TcpConnectionPool);
		m_EventSystem = std::make_unique<EventSystem>();

		m_TcpConnectionManager->InitializeServerSocket();
	}

	void Start()
	{
		while (1)
		{
			m_TcpConnectionManager->TryAcceptIncomingConnection();
			m_TcpMessageReceiver->TryReceiveMessage([](std::string message) {
				std::cout << "Received message: " << message << '\n';
			});

			sleep(1); // Release current thread so we don't overwork the thread for now
		}
	}

private:
	std::unique_ptr<TcpConnectionPool> m_TcpConnectionPool;
	std::unique_ptr<TcpSocketConnectionManager> m_TcpConnectionManager;
	std::unique_ptr<TcpSocketMessenger> m_TcpSocketMessenger;
	std::unique_ptr<TcpMessageReceiver> m_TcpMessageReceiver;
	std::unique_ptr<EventSystem> m_EventSystem;
};
