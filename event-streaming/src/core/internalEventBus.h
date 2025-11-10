#pragma once

#include <map>
#include <functional>
#include "internalEventTypes.h"

class InternalEventBus
{
public:
	void ProduceEvent(IInternalEvent* event)
	{
		LOG_DEBUG("Producing internal event of type {}", static_cast<int>(event->GetType()));
		auto it = m_Subscribers.find(event->GetType());
		if (it != m_Subscribers.end())
		{
			for (auto& fn : it->second)
			{
				fn(event);
			}
		}
		delete(event);
	}

	void AddCallback(InternalEventType eventType, std::function<void(IInternalEvent*)> fn)
	{
		LOG_DEBUG("Adding internal event callback for event type {}", static_cast<int>(eventType));
		
		auto it = m_Subscribers.find(eventType);
		if (it != m_Subscribers.end())
		{
			it->second.push_back(fn);
		}

		else
		{
			m_Subscribers[eventType] = { fn };
		}
	}
private:
	std::map<InternalEventType, std::vector<std::function<void(IInternalEvent* event)>>> m_Subscribers;
};