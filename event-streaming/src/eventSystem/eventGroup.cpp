#include <map>
#include "eventGroup.h"

EventGroup::EventGroup(GroupId groupId)
	: EventGroupId(groupId)
{
}
std::optional<SocketType> EventGroup::GetNextSocket()
{
	if (Sockets.size() == 0)
		return std::nullopt;
	SocketType sock = Sockets[m_NextSocketIndex];
	if (m_NextSocketIndex + 1 < Sockets.size())
		m_NextSocketIndex++;
	else
		m_NextSocketIndex = 0;

	return std::optional<SocketType>(sock);
}