#pragma once

#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>
#include "event.h"
#include "../networking/linux/tcpSocketMessenger.h"

class EventSystem
{
public:
	explicit EventSystem(TcpSocketMessenger& tcpSocketMessenger);
	void RegisterEventType(std::unique_ptr<EventType>&& eventType);
	void Subscribe(std::string eventType, unsigned int socket);
	void Unsubscribe(const std::string& eventType, unsigned int socket);
	void ProduceEvent(Event&& event);
private:
	void Publish(Event& event);
	std::string FormMessage(Event& event);
private:
	// event type name t-> ipAddresses
	// TODO: linked list may be better for performance on higher loads
	TcpSocketMessenger& m_TcpSocketMessenger;
	std::vector<Event> m_Events;
	std::unordered_map<std::string, std::unique_ptr<EventType>> m_EventTypes;
	std::unordered_map<std::string, std::set<unsigned int>> m_Subscribers;
};