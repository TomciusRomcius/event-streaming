#include "event.h"

EventType::EventType(std::string name, std::unordered_map<std::string, PropertyType>&& properties)
    : m_Name(name), m_Properties(std::move(properties)) { }

// Checks if all properties are present in the given values map.
bool EventType::ValidateEventValues(const std::unordered_map<std::string, std::unique_ptr<IProperty>>& values)
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

Event::Event(std::string eventType, std::unordered_map<std::string, std::unique_ptr<IProperty>>&& values)
		: m_EventType(eventType), m_Values(std::move(values)) { }

Event::Event(Event &&other)
    : m_EventType(std::move(other.m_EventType)), m_Values(std::move(other.m_Values))
{
}