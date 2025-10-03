#pragma once

#include <map>
#include <stdexcept>
#include <string>
#include <memory>
#include "eventProperty.h"
#include <unordered_map>

class EventType
{
public:
	explicit EventType(std::string name, std::unordered_map<std::string, PropertyType> &&properties);
	// Checks if all properties are present in the given values map.
	bool ValidateEventValues(const std::unordered_map<std::string, std::unique_ptr<IProperty>> &values);
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
	Event(std::string eventType, std::unordered_map<std::string, std::unique_ptr<IProperty>> &&values);
	Event(Event &&other);
	inline std::string GetName() const
	{
		return m_EventType;
	}
	inline const std::unordered_map<std::string, std::unique_ptr<IProperty>> &GetProperties() const
	{
		return m_Values;
	}
private:
	std::string m_EventType;
	std::unordered_map<std::string, std::unique_ptr<IProperty>> m_Values;
};