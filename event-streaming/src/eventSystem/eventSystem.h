#pragma once

#include "event.h"
#include <iostream>
#include <vector>

class EventSystem
{
public:
	EventSystem() = default;

	void RegisterEventType(std::unique_ptr<EventType>&& eventType)
	{
		if (m_Subscribers.find(eventType->GetName()) != m_Subscribers.end())
		{
			throw std::runtime_error("Event type already registered.");
		}
		
		m_Subscribers[eventType->GetName()] = {};
		m_EventTypes[eventType->GetName()] = std::move(eventType);
	}

	void Subscribe(std::string eventType, std::string ipAddress)
	{
		auto it = m_Subscribers.find(eventType);
		if (it == m_Subscribers.end())
		{
			throw std::invalid_argument("Event type has not been registered");
		}
		else
		{
			it->second.push_back(ipAddress);
		}
	}

	void Unsubscribe(EventType& eventType, std::string ipAddress)
	{
	}

	void ProduceEvent(Event&& event)
	{
		// TODO: store
		if (m_EventTypes.find(event.GetName()) == m_EventTypes.end())
		{
			std::cout << "WARNING: trying to produce event when the event type does not exist " << '\n';
			return;
		}

		m_Events.push_back(std::move(event));
		Publish(*m_Events.rbegin());
	}

private:
	void Publish(Event& event)
	{
		auto it = m_Subscribers.find(event.GetName());
		if (it == m_Subscribers.end())
		{
			throw std::runtime_error("Event type has not been registered!");
		}

		for (std::string ipAddress : it->second)
		{
			std::cout << "Sending event to: " << ipAddress << '\n';
		}
	}

private:
	// event type name -> ipAddresses
	// TODO: linked list may be better for performance on higher loads
	std::vector<Event> m_Events;
	std::unordered_map<std::string, std::unique_ptr<EventType>> m_EventTypes;
	std::unordered_map<std::string, std::vector<std::string>> m_Subscribers;
};