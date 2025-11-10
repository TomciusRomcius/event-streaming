#pragma once
#include <set>
#include <functional>
#include "../../application/logging.h"
#include "../../core/internalEventBus.h"

#ifdef WIN32
typedef SOCKET SocketType;
#endif
#ifdef __linux__
typedef int SocketType;
#endif

struct TcpConnectionPool
{
public:
	TcpConnectionPool(InternalEventBus& internalEventBus)
		: m_InternalEventBus(internalEventBus)
	{
	}

	inline void AddClientSocket(SocketType socket)
	{
		LOG_DEBUG("Adding socket '{}' to the connection pool", socket);
		m_ClientSockets.insert(socket);
		m_InternalEventBus.ProduceEvent(new ClientConnectedEvent(socket));
	}

	inline void RemoveClientSocket(SocketType socket)
	{
		LOG_DEBUG("Removing socket '{}' from the connection pool", socket);
		m_ClientSockets.erase(socket);
		m_InternalEventBus.ProduceEvent(new ClientDisconnectedEvent(socket));
	}

	inline bool HasClientSocket(SocketType socket) const
	{
		return m_ClientSockets.find(socket) != m_ClientSockets.end();
	}

	inline int GetHighestSocketDescriptor() const
	{
		if (m_ClientSockets.empty())
			return -1;

		return *m_ClientSockets.rbegin(); // Return the largest socket descriptor
	}

	inline bool Empty() const { return m_ClientSockets.empty(); }

	inline std::set<SocketType> GetClientSockets() { return m_ClientSockets; }
private:
	InternalEventBus& m_InternalEventBus;
	std::set<SocketType> m_ClientSockets;
};