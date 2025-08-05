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
#include <nlohmann/json.hpp>
#include "tcpRequestHandlerService.h"

std::string GetTcpRequestType(nlohmann::json json)
{
	if (!json.contains("type"))
	{
		std::cerr << "Failed to get TCP request type" << '\n';
		return "err";
	}

	if (!json["type"].is_string())
	{
		std::cerr << "Failed to get TCP request type" << '\n';
		return "err";	
	}
	
	return json["type"];
}

class Application
{
public:
	Application()
	{
		unsigned int serverPort = 9000;
		m_EventSystem = std::make_unique<EventSystem>();
		m_TcpConnectionPool = std::make_unique<TcpConnectionPool>();

		m_TcpConnectionManager = std::make_unique<TcpSocketConnectionManager>(*m_TcpConnectionPool, serverPort);
		m_TcpConnectionManager->InitializeServerSocket();

		m_TcpSocketMessenger = std::make_unique<TcpSocketMessenger>(*m_TcpConnectionPool);
		m_TcpMessageReceiver = std::make_unique<TcpMessageReceiver>(*m_TcpConnectionPool);
		m_TcpRequestHandlerService = std::make_unique<TcpRequestHandlerService>();
		m_EventSystem = std::make_unique<EventSystem>();

		RegisterRequestStrategies();
	}

	void RegisterRequestStrategies()
	{
		// Strategies are owned by handler service
		m_TcpRequestHandlerService->RegisterStrategy(
			"create-event-type",
			new CreateEventTypeHandler(*m_EventSystem)
		);
	}

	void Start()
	{
		while (1)
		{
			m_TcpConnectionManager->TryAcceptIncomingConnection();
			m_TcpMessageReceiver->TryReceiveMessage([&](std::string message) {
				std::cout << "TryReceiveMessage lambda" << '\n';
				auto js = nlohmann::json::parse(message);
				std::string type = GetTcpRequestType(js);
				std::cout << "type: " << type << '\n';
				m_TcpRequestHandlerService->TryExecuteStrategy(type, js);
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
	std::unique_ptr<TcpRequestHandlerService> m_TcpRequestHandlerService;
};
