#include <algorithm>
#include "eventSystem.h"

EventSystem::EventSystem(TcpSocketMessenger& tcpSocketMessenger)
	: m_TcpSocketMessenger(tcpSocketMessenger)
{
}

void EventSystem::RegisterEventType(std::unique_ptr<EventType>&& eventType)
{
	LOG_TRACE("Entered EventSystem::RegisterEventType");
	LOG_INFO("Registering event: '{}'", eventType->GetName());

	if (m_EventTypes.find(eventType->GetName()) != m_EventTypes.end())
	{
		LOG_ERROR("{} event type has already been registered!", eventType->GetName());
		return;
	}

	m_EventTypes[eventType->GetName()] = std::move(eventType);
}
void EventSystem::Subscribe(std::string& eventType, GroupId groupId, SocketType socket)
{
	LOG_TRACE("Entered EventSystem::Subscribe");
	LOG_DEBUG("Subscribing socket: {} to event type: {}", socket, eventType);
	auto it = m_Groups.find(eventType);

	if (it == m_Groups.end())
	{
		auto evGroup = EventGroup(groupId);
		evGroup.Sockets.push_back(socket);
		m_Groups[eventType] = { std::move(evGroup)};
	}
	else
	{
		auto& groups = it->second;
		auto groupIt = std::find_if(groups.begin(), groups.end(), [groupId](EventGroup& group)
			{
				return group.EventGroupId == groupId;
			});
		if (groupIt != groups.end())
		{
			groupIt->Sockets.push_back(socket);
		}
		else
		{
			auto evGroup = EventGroup(groupId);
			evGroup.Sockets.push_back(socket);
			groups.push_back(std::move(evGroup));
		}
	}
}

void EventSystem::Unsubscribe(std::string& eventType, GroupId groupId, SocketType socket)
{
	LOG_TRACE("Entered EventSystem::Unsubscribe");
	auto groupsIt = m_Groups.find(eventType);
	if (groupsIt == m_Groups.end())
	{
		LOG_ERROR(
			"Failed unsubscribe socket {} from event type '{}' and group '{}': event type or group does not exist",
			socket,
			groupId,
			eventType
		);
		return;
	}
	
	auto& groups = groupsIt->second;
	auto groupIt = std::find_if(groups.begin(), groups.end(), [groupId](EventGroup& group)
		{
			return group.EventGroupId == groupId;
		});
	if (groupIt == groups.end())
		return;

	auto& eventGroup = *groupIt;
	auto socketIt = std::find(eventGroup.Sockets.begin(), eventGroup.Sockets.end(), socket);
	if (socketIt != eventGroup.Sockets.end())
	{
		eventGroup.Sockets.erase(socketIt);
	}
}

void EventSystem::ProduceEvent(Event&& event)
{
	LOG_TRACE("Entered EventSystem::ProduceEvent");
	LOG_DEBUG("Producing event: {}", event.GetName());
	// TODO: store
	if (m_EventTypes.find(event.GetName()) == m_EventTypes.end())
	{
		LOG_ERROR("ProduceEvent failed: event type '{}' does not exist!", event.GetName());
		return;
	}

	m_Events.push_back(std::move(event));
	Publish(*m_Events.rbegin());
}

void EventSystem::Publish(Event& event)
{
	LOG_TRACE("Entered EventSystem::Publish");
	LOG_DEBUG("Publishing event type: '{}'", event.GetName());

	auto it = m_Groups.find(event.GetName());
	if (it == m_Groups.end())
	{
		throw std::runtime_error("Publish failed: event type has not been registered!");
	}

	// TODO extract group load balancing logic and implement round robin instead of random
	std::string formedMessage = FormMessage(event);
	std::vector<SocketType> sockets;
	sockets.reserve(it->second.size());
	for (auto& group : it->second)
	{
		srand(time(0));
		SocketType sock = group.Sockets[rand() % group.Sockets.size()];
		LOG_DEBUG("Sending event '{}' to socket '{}'", event.GetName(), sock);
		m_TcpSocketMessenger.QueueMessage({ sock }, formedMessage);
	}
}

std::string EventSystem::FormMessage(Event& event)
{
	const std::unordered_map<std::string, std::unique_ptr<IProperty>>& props = event.GetProperties();
	nlohmann::json jsonMessage;
	for (const auto& entry : props)
	{
		IProperty* property = entry.second.get();
		PropertyType propertyType = property->GetPropertyType();
		if (propertyType == PropertyType::STRING)
		{
			auto strProperty = dynamic_cast<StringProperty*>(property);
			if (!strProperty)
			{
				continue;
			}
			jsonMessage[entry.first] = strProperty->GetValue();
		}
		else if (propertyType == PropertyType::NUMBER)
		{
			auto numProperty = dynamic_cast<NumberProperty*>(property);
			if (!numProperty)
			{
				continue;
			}
			jsonMessage[entry.first] = numProperty->GetValue();
		}
		else if (propertyType == PropertyType::BOOLEAN)
		{
			auto boolProperty = dynamic_cast<BooleanProperty*>(property);
			if (!boolProperty)
			{
				continue;
			}
			jsonMessage[entry.first] = boolProperty->GetValue();
		}
	}

	return nlohmann::to_string(jsonMessage);
}

void EventSystem::HandleClientDisconnect(IInternalEvent* event)
{
	ClientDisconnectedEvent* disconnectedEvent = static_cast<ClientDisconnectedEvent*>(event);
	auto it = m_SocketToSubscriberInfo.find(disconnectedEvent->Socket);
	if (it == m_SocketToSubscriberInfo.end())
	{
		return;
	}

	SubscriberInfo& info = it->second;
	for (auto& [key, value] : info.SubscribedEventTypes)
	{
		auto groupIt = m_Groups.find(key);
		if (groupIt == m_Groups.end())
			continue;

		for (auto& [eventType, groupIds] : info.SubscribedEventTypes)
		{
			auto groupsIt = m_Groups.find(eventType);
			if (groupsIt == m_Groups.end())
				break;

			auto& groups = groupsIt->second;

			// TODO: Depending on the amount of groups the algorithm
			// could be more efficient as we are looping over a non-sorted vector of n groups
			for (auto groupId : groupIds)
			{
				auto targetGroupIt = std::find_if(groups.begin(), groups.end(), [groupId](EventGroup& group)
				{
					return group.EventGroupId == groupId;
				});
				groupsIt->second.erase(targetGroupIt);
			}
		}
	}

	m_SocketToSubscriberInfo.erase(disconnectedEvent->Socket);
}

