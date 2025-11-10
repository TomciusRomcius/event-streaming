#pragma once
#include "../networking/shared/types.h"

enum class InternalEventType
{
	CLIENT_CONNECTED,
	CLIENT_DISCONNECTED,
};

class IInternalEvent
{
public:
	virtual inline InternalEventType GetType() = 0;
};

class ClientConnectedEvent : public IInternalEvent
{
public:
	ClientConnectedEvent(SocketType clientSocket)
		: Socket(clientSocket)
	{
	}

	inline InternalEventType GetType() override
	{
		return InternalEventType::CLIENT_CONNECTED;
	}

	SocketType Socket;
};

class ClientDisconnectedEvent : public IInternalEvent
{
public:
	ClientDisconnectedEvent(SocketType clientSocket)
		: Socket(clientSocket)
	{
	}

	inline InternalEventType GetType() override
	{
		return InternalEventType::CLIENT_DISCONNECTED;
	}

	SocketType Socket;
};