#include <memory>
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include "../networking/linux/tcpSocketConnectionManager.h"
#include "../networking/shared/tcpSocketMessenger.h"
#include "../networking/linux/tcpMessageReceiver.h"
#include "../networking/shared/tcpRequest.h"
#include "../eventSystem/eventSystem.h"
#include "tcpRequestHandlerService.h"
#include "utils.h"
#include "application.h"

Application::Application()
{
    spdlog::set_level(spdlog::level::trace);
    SPDLOG_TRACE("Entered Application::Application");
    
    unsigned int serverPort = 9000;
    m_TcpConnectionPool = std::make_unique<TcpConnectionPool>();

    m_TcpConnectionManager = std::make_unique<TcpSocketConnectionManager>(*m_TcpConnectionPool, serverPort);
    m_TcpConnectionManager->InitializeServerSocket();

    m_TcpSocketMessenger = std::make_unique<TcpSocketMessenger>(*m_TcpConnectionPool);
    m_TcpMessageReceiver = std::make_unique<TcpMessageReceiver>(*m_TcpConnectionManager, *m_TcpConnectionPool);
    m_TcpRequestHandlerService = std::make_unique<TcpRequestHandlerService>();
    m_EventSystem = std::make_unique<EventSystem>(*m_TcpSocketMessenger);

    RegisterRequestStrategies();
}

void Application::RegisterRequestStrategies()
{
    LOG_TRACE("Entered Application::RegisterRequestStrategies");
    // Strategies are owned by handler service
    m_TcpRequestHandlerService->RegisterStrategy(
        "create-event-type",
        std::make_unique<CreateEventTypeHandler>(*m_EventSystem)
    );
    m_TcpRequestHandlerService->RegisterStrategy(
        "produce-event",
        std::make_unique<ProduceEventHandler>(*m_EventSystem)
    );

    m_TcpRequestHandlerService->RegisterStrategy(
        "subscribe-to-event-type",
        std::make_unique<SubscribeToEventHandler>(*m_EventSystem)
    );

    m_TcpRequestHandlerService->RegisterStrategy(
        "unsubscribe-from-event-type",
        std::make_unique<UnsubscribeFromEventHandler>(*m_EventSystem)
    );
}

void Application::Start()
{
    LOG_TRACE("Entered Application::Start");
    while (true)
    {
        m_TcpConnectionManager->TryAcceptIncomingConnection();
        m_TcpMessageReceiver->TryReceiveMessage([this](std::string message, unsigned int socket) 
        { 
            HandleTcpMessage(message, socket); 
        });
        m_TcpSocketMessenger->Update();
    }
}

void Application::HandleTcpMessage(const std::string& message, unsigned int socket)
{
    LOG_TRACE("Entered Application::HandleTcpMessage");
    try 
    {
        auto js = nlohmann::json::parse(message);
        std::string type = GetTcpRequestType(js);
        LOG_DEBUG("Request type: '{}'", type);
        m_TcpRequestHandlerService->TryExecuteStrategy(type, TcpRequest(socket, js));
    }
    catch (nlohmann::json_abi_v3_12_0::detail::type_error ex)
    {
        LOG_ERROR(ex.what());
    }
    
    catch (const std::exception& e) {
        std::cerr << "std::exception: " << e.what() << '\n';
    }	

    catch (...)
    {
        std::cerr << "Unknown exception was thrown while executing strategy" << '\n';
    }
}
