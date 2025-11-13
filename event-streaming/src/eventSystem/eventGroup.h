#pragma once

#include <map>
#include "../networking/shared/types.h"

typedef uint16_t GroupId;

class EventGroup
{
public:
	EventGroup(GroupId groupId);
	// Used for round robin loadbalancing. Returns next socket
	std::optional<SocketType> GetNextSocket();
public:
	GroupId EventGroupId;
	std::vector<SocketType> Sockets;
private:
	size_t m_NextSocketIndex = 0;
};