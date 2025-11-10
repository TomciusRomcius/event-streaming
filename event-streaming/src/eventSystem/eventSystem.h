#pragma once

#include <vector>
#include <nlohmann/json.hpp>
#include <set>
#include <random>
#include "event.h"
#include "groupContainer.h"
#include "../networking/shared/tcpSocketMessenger.h"

class TcpSocketMessenger;

// Used for clean up on socket disconnect
struct SubscriberInfo
{
	SocketType Socket;
	std::map<std::string, std::vector<GroupId>> SubscribedEventTypes;
};

class EventSystem
{
public:
	explicit EventSystem(TcpSocketMessenger& tcpSocketMessenger);
	void RegisterEventType(std::unique_ptr<EventType>&& eventType);
	void Subscribe(std::string& eventType, GroupId groupId, SocketType socket);
	void Unsubscribe(std::string& eventType, GroupId groupId, SocketType socket);
	void ProduceEvent(Event&& event);
	void HandleClientDisconnect(IInternalEvent* event);
private:
	void Publish(Event& event);
	std::string FormMessage(Event& event);
private:
	TcpSocketMessenger& m_TcpSocketMessenger;
	std::vector<Event> m_Events;
	std::unordered_map<std::string, std::unique_ptr<EventType>> m_EventTypes;
	std::unordered_map<std::string, std::vector<EventGroup>> m_Groups;
	std::map<SocketType, SubscriberInfo> m_SocketToSubscriberInfo;

	// SocketType -> SubscriberInfo(subscribed event types and groups)
};
