#pragma once

#include <vector>
#include <nlohmann/json.hpp>
#include <set>
#include <random>
#include "event.h"
#include "groupContainer.h"
#include "../networking/shared/tcpSocketMessenger.h"

class TcpSocketMessenger;

class EventSystem
{
public:
	explicit EventSystem(TcpSocketMessenger& tcpSocketMessenger);
	void RegisterEventType(std::unique_ptr<EventType>&& eventType);
	void Subscribe(std::string& eventType, GroupId groupId, SocketType socket);
	void Unsubscribe(std::string& eventType, GroupId groupId, SocketType socket);
	void ProduceEvent(Event&& event);
private:
	void Publish(Event& event);
	std::string FormMessage(Event& event);
private:
	// event type name t-> ipAddresses
	TcpSocketMessenger& m_TcpSocketMessenger;
	std::vector<Event> m_Events;
	std::unordered_map<std::string, std::unique_ptr<EventType>> m_EventTypes;
	std::unordered_map<std::string, GroupSocketsContainer> m_Groups;
};
