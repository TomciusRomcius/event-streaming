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
		m_Groups[eventType] = GroupSocketsContainer();
		m_Groups.at(eventType).AddSocket(groupId, socket);
	}
	else
	{
		it->second.AddSocket(groupId, socket);
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
	if (groupsIt->second.RemoveSocketFromGroup(groupId, socket))
	{
		LOG_INFO("Unsubscribed socket {} from event type '{}'", socket, eventType);
	}

	else
	{
		LOG_WARN(
			"Socket unsubscribe failed: Socket {} is not subscribed to '{}' event type",
			socket,
			eventType
		);
		return;
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

	std::vector<SocketType> sockets = it->second.GetSockets();
	std::string formedMessage = FormMessage(event);

	for (unsigned int socket : sockets)
	{
		LOG_DEBUG("Sending event '{}' to socket '{}'", event.GetName(), socket);
		m_TcpSocketMessenger.QueueMessage({ socket }, formedMessage);
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
