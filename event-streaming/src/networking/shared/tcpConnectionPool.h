#pragma once
#include <set>
#include "../../application/logging.h"

#ifdef WIN32
typedef SOCKET SocketType;
#endif
#ifdef __linux__
typedef int SocketType;
#endif

struct TcpConnectionPool
{
public:
	inline void AddClientSocket(SocketType socket)
	{
		LOG_DEBUG("Adding socket '{}' to the connection pool", socket);
		m_ClientSockets.insert(socket);
	}

	inline void RemoveClientSocket(SocketType socket)
	{
		LOG_DEBUG("Removing socket '{}' from the connection pool", socket);
		m_ClientSockets.erase(socket);
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
	std::set<SocketType> m_ClientSockets;
};