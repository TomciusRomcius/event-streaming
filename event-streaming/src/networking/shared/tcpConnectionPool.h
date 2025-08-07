#pragma once
#include <set>
#include "../../application/logging.h"

struct TcpConnectionPool
{
public:
	inline void AddClientSocket(unsigned int socket)
	{
		LOG_DEBUG("Adding socket '{}' to the connection pool", socket);
		clientSockets.insert(socket);
	}

	inline void RemoveClientSocket(unsigned int socket)
	{
		LOG_DEBUG("Removing socket '{}' from the connection pool", socket);
		clientSockets.erase(socket);
	}

	inline bool HasClientSocket(unsigned int socket) const
	{
		return clientSockets.find(socket) != clientSockets.end();
	}

	inline int GetHighestSocketDescriptor() const
	{
		if (clientSockets.empty())
			return -1;

		return *clientSockets.rbegin(); // Return the largest socket descriptor
	}

	auto begin() const
	{
		return clientSockets.begin();
	}

	auto end() const
	{
		return clientSockets.end();
	}
private:
	std::set<unsigned int> clientSockets;
};