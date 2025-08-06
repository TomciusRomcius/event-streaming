#pragma once

#include <map>
#include <stdexcept>
#include <string>
#include <memory>
#include "eventProperty.h"

class EventType
{
public:
	EventType(std::string name, std::unordered_map<std::string, PropertyType>&& properties)
		: m_Name(name), m_Properties(std::move(properties))
	{

	}

	// Checks if all properties are present in the given values map.
	bool ValidateEventValues(const std::unordered_map<std::string, std::unique_ptr<IProperty>>& values)
	{
		// TODO: more robust, check for types
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
	std::unordered_map<std::string, PropertyType> m_Properties;
};

class Event
{
public:
	Event(std::string eventType, std::unordered_map<std::string, std::unique_ptr<IProperty>>&& values)
		: m_EventType(eventType), m_Values(std::move(values))
	{ }

	inline std::string GetName() const
	{
		return m_EventType;
	}

private:
	std::string m_EventType;
	std::unordered_map<std::string, std::unique_ptr<IProperty>> m_Values;
};