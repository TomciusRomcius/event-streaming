#pragma once

#include <map>
#include "../networking/shared/types.h"

typedef uint16_t GroupId;

class GroupSocketsContainer
{
public:
	GroupSocketsContainer()
	{
	}

	void AddSocket(GroupId groupId, SocketType socket)
	{
		LOG_TRACE("Entered AddSocket");
		LOG_INFO("Adding socket '{}' to group '{}'", socket, groupId);
		auto it = m_GroupToSockets.find(groupId);

		if (it == m_GroupToSockets.end())
		{
			m_GroupToSockets[groupId] = std::set<SocketType>{ socket };
		}

		else
		{
			it->second.insert(groupId);
		}
	}

	// Returns true if socket was removed, false if socket does not exist in the group
	bool RemoveSocketFromGroup(GroupId groupId, SocketType socket)
	{
		LOG_TRACE("Entered RemoveSocketFromGroup");
		LOG_INFO("Removing socket '{}' from group ''", socket, groupId);
		auto groupIt = m_GroupToSockets.find(groupId);
		if (groupIt == m_GroupToSockets.end())
		{
			return false;
		}

		return groupIt->second.erase(socket) > 0;
	}

	// TODO: prevent copy
	std::vector<SocketType> GetSockets() const
	{
		LOG_TRACE("Entered GetSockets");
		std::vector<SocketType> result(m_GroupToSockets.size());

		for (auto& socketSet : m_GroupToSockets)
		{
			std::vector<SocketType> socketArr(socketSet.second.size());
			for (auto& socket : socketSet.second)
			{
				socketArr.push_back(socket);
			}

			// Retrieve random socket from the group
			// TODO: different load balancing modes
			srand(time(0));
			result.push_back(socketArr[rand() % socketArr.size()]);
		}

		return result;
	}

private:
	std::map<GroupId, std::set<SocketType>> m_GroupToSockets;
};