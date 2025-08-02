#pragma once

#include "event.h"
#include <iostream>
#include <vector>

class EventSystem
{
public:
	EventSystem()
	{

	}

	void RegisterEventType(EventType& eventType)
	{
		if (m_Subscribers.find(eventType.GetName()) != m_Subscribers.end())
		{
			throw std::runtime_error("Event type already registered.");
		}
		m_Subscribers[eventType.GetName()] = {};
	}

	void Subscribe(EventType& eventType, std::string ipAddress)
	{
		auto it = m_Subscribers.find(eventType.GetName());
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

	void ProduceEvent(const Event& event) const
	{
		// TODO: store

		Publish(event);
	}

private:
	void Publish(const Event& event) const
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
	std::map<std::string, std::vector<std::string>> m_Subscribers;
};