#include <map>
#include <functional>
#include "internalEventTypes.h"

class InternalEventBus
{
public:
	void ProduceEvent(IInternalEvent* event)
	{
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

	void AddCallback(InternalEventType eventType, std::function<void(IInternalEvent* event)> fn)
	{
		auto it = m_Subscribers.find(eventType);
		if (it != m_Subscribers.end())
		{
			it->second.push_back(fn);
		}
	}
private:
	std::map<InternalEventType, std::vector<std::function<void(IInternalEvent* event)>>> m_Subscribers;
};