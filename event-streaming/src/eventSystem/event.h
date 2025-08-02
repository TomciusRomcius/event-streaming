#pragma once

#include <map>
#include <stdexcept>
#include <string>

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