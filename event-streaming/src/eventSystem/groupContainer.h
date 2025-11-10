#pragma once

#include <map>
#include "../networking/shared/types.h"

typedef uint16_t GroupId;

struct EventGroup
{
	EventGroup(GroupId groupId)
		: EventGroupId(groupId)
	{ }
	GroupId EventGroupId;
	std::vector<SocketType> Sockets;
};