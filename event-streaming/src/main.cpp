#include <map>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

enum class PropertyType
{
	STRING,
	INT,
	DECIMAL
};

class EventType
{
public:
	EventType(std::string name, std::map<std::string, PropertyType>& properties)
		: m_Name(name), m_Properties(properties)
	{

	}

	// Checks if all properties are present in the given values map.
	bool ValidateEventValues(const std::map<std::string, void*>& values)
	{
		for (const auto& property : m_Properties)
		{
			auto it = values.find(property.first);
			if (it == values.end())
			{
				return false; // Missing property
			}
		}

		return true;
	}

	inline std::string GetName() const
	{
		return m_Name;
	}

private:
	std::string m_Name;
	std::map<std::string, PropertyType> m_Properties;
};

class Event
{
public:
	// TODO: using void* may be dangerous, later implement a type-safe variant
	Event(EventType& type, std::map<std::string, void*>& values)
		: m_EventType(type), m_Values(values)
	{
		if (!m_EventType.ValidateEventValues(values))
		{
			throw std::invalid_argument("Invalid event values provided.");
		}
	}

	inline std::string GetName() const
	{
		return m_EventType.GetName();
	}

private:
	EventType& m_EventType;
	std::map<std::string, void*>& m_Values;
};

class EventStreamer
{
public:
	EventStreamer()
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

int main()
{
	auto streamer = EventStreamer();
	std::map<std::string, PropertyType> properties = {
		{ "key1", PropertyType::INT },
		{ "key2", PropertyType::DECIMAL }
	};

	auto eventType = EventType("TestEvent", properties);

	streamer.RegisterEventType(eventType);
	streamer.Subscribe(eventType, "127.0.0.1");

	std::map<std::string, void*> values = {
		{ "key1", (void*)new int(42) },
		{ "key2", (void*)new double(42.2) },
	};
	auto testEvent = Event(
		eventType,
		values
	);
	streamer.ProduceEvent(testEvent);
	return 0;
}
