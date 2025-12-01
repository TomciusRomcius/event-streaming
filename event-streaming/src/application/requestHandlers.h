#pragma once

#include "../eventSystem/eventSystem.h"
#include "../networking/shared/tcpRequest.h"

class ITcpRequestHandler
{
public:
    virtual void Execute(TcpRequest request) = 0;
    virtual ~ITcpRequestHandler() = default;
};

class CreateEventTypeHandler : public ITcpRequestHandler
{
public:
    explicit CreateEventTypeHandler(EventSystem& eventSystem);
    void Execute(TcpRequest request) override;
private:
    EventSystem& m_EventSystem;
};

class ProduceEventHandler : public ITcpRequestHandler
{
public:
    explicit ProduceEventHandler(EventSystem& eventSystem);
    void Execute(TcpRequest request) override;
private:
    EventSystem& m_EventSystem;
};

class SubscribeToEventHandler : public ITcpRequestHandler
{
public:
    explicit SubscribeToEventHandler(EventSystem& eventSystem);
    void Execute(TcpRequest request) override;
private:
    EventSystem& m_EventSystem;
};

class UnsubscribeFromEventHandler : public ITcpRequestHandler
{
public:
    explicit UnsubscribeFromEventHandler(EventSystem& eventSystem);
    void Execute(TcpRequest request) override;
private:
    EventSystem& m_EventSystem;
};
