#pragma once

#include "event.h"
#include "../application/logging.h"
#include "../networking/linux/tcpSocketMessenger.h"
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>

class EventSystem
{
public:
	explicit EventSystem(TcpSocketMessenger& tcpSocketMessenger)
		: m_TcpSocketMessenger(tcpSocketMessenger)
	{ }

	void RegisterEventType(std::unique_ptr<EventType>&& eventType)
	{
		LOG_TRACE("Entered EventSystem::RegisterEventType");
		LOG_INFO("Registering event: '{}'", eventType->GetName());

		if (m_Subscribers.find(eventType->GetName()) != m_Subscribers.end())
		{
			LOG_ERROR("{} event type has already been registered!", eventType->GetName());
			return;
		}
		
		m_Subscribers[eventType->GetName()] = {};
		m_EventTypes[eventType->GetName()] = std::move(eventType);
	}

	void Subscribe(std::string eventType, unsigned int socket)
	{
		LOG_TRACE("Entered EventSystem::Subscribe");
		LOG_DEBUG("Subscribing socket: {} to event type: {}", socket, eventType);

		auto it = m_Subscribers.find(eventType);
		if (it == m_Subscribers.end())
		{
			LOG_ERROR(
				"Subscription failed: event type '{}' not found for socket '{}'",
				eventType,
				socket
			);
			return;
		}
		else
		{
			LOG_DEBUG("Pushing host to m_Subscribers list");
			it->second.push_back(socket);
		}
	}

	void Unsubscribe(const EventType& eventType, unsigned int socket)
	{
		LOG_TRACE("Entered EventSystem::Unsubscribe");
	}

	void ProduceEvent(Event&& event)
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

private:
	void Publish(Event& event)
	{
		LOG_TRACE("Entered EventSystem::Publish");
		LOG_DEBUG("Publishing event type: '{}'", event.GetName());
		auto it = m_Subscribers.find(event.GetName());
		if (it == m_Subscribers.end())
		{
			throw std::runtime_error("Publish failed: fvent type has not been registered!");
		}

		for (unsigned int socket : it->second)
		{
			LOG_DEBUG("Sending event '{}' to socket '{}'", event.GetName(), socket);
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
			m_TcpSocketMessenger.SendRequest({socket}, nlohmann::to_string(jsonMessage));
		}
	}

private:
	// event type name -> ipAddresses
	// TODO: linked list may be better for performance on higher loads
	const TcpSocketMessenger& m_TcpSocketMessenger;
	std::vector<Event> m_Events;
	std::unordered_map<std::string, std::unique_ptr<EventType>> m_EventTypes;
	std::unordered_map<std::string, std::vector<unsigned int>> m_Subscribers;
};