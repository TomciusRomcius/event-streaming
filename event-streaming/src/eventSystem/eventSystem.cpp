#include "eventSystem.h"

EventSystem::EventSystem(TcpSocketMessenger& tcpSocketMessenger)
		: m_TcpSocketMessenger(tcpSocketMessenger)
	{ }

void EventSystem::RegisterEventType(std::unique_ptr<EventType>&& eventType)
{
    LOG_TRACE("Entered EventSystem::RegisterEventType");
    LOG_INFO("Registering event: '{}'", eventType->GetName());

    if (m_Subscribers.find(eventType->GetName()) != m_Subscribers.end())
    {
        LOG_ERROR("{} event type has already been registered!", eventType->GetName());
        return;
    }
    
    m_Subscribers[eventType->GetName()] = {};
    m_EventTypes[eventType->GetName()] = std::move(eventType);
}

void EventSystem::Subscribe(std::string eventType, unsigned int socket)
{
    LOG_TRACE("Entered EventSystem::Subscribe");
    LOG_DEBUG("Subscribing socket: {} to event type: {}", socket, eventType);

    auto it = m_Subscribers.find(eventType);
    if (it == m_Subscribers.end())
    {
        LOG_ERROR(
            "Subscription failed: event type '{}' not found for socket '{}'",
            eventType,
            socket
        );
        return;
    }
    else
    {
        LOG_DEBUG("Pushing host to m_Subscribers list");
        it->second.insert(socket);
    }
}

void EventSystem::Unsubscribe(const std::string& eventType, unsigned int socket)
{
    LOG_TRACE("Entered EventSystem::Unsubscribe");
    auto subsIt = m_Subscribers.find(eventType);
    if (subsIt == m_Subscribers.end())
    {
        LOG_ERROR(
            "Failed unsubscribe socket {} from event type '{}': event type does not",
            socket,
            eventType
        );
        return;
    }
    if (!subsIt->second.contains(socket))
    {
        LOG_WARN(
            "Socket unsubscribe failed: Socket {} is not subscribed to '{}' event type",
            socket,
            eventType
        );
        return;
    }

    subsIt->second.erase(socket);
    LOG_DEBUG("Unsubscribed socket {} from event type '{}'", socket, eventType);
}

void EventSystem::ProduceEvent(Event&& event)
{
    LOG_TRACE("Entered EventSystem::ProduceEvent");
    LOG_DEBUG("Producing event: {}", event.GetName());
    // TODO: store
    if (m_EventTypes.find(event.GetName()) == m_EventTypes.end())
    {
        LOG_ERROR("ProduceEvent failed: event type '{}' does not exist!", event.GetName());
        return;
    }

    m_Events.push_back(std::move(event));
    Publish(*m_Events.rbegin());
}

void EventSystem::Publish(Event& event)
{
    LOG_TRACE("Entered EventSystem::Publish");
    LOG_DEBUG("Publishing event type: '{}'", event.GetName());
    auto it = m_Subscribers.find(event.GetName());
    if (it == m_Subscribers.end())
    {
        throw std::runtime_error("Publish failed: fvent type has not been registered!");
    }

    for (unsigned int socket : it->second)
    {
        LOG_DEBUG("Sending event '{}' to socket '{}'", event.GetName(), socket);
        std::string formedMessage = FormMessage(event);
        m_TcpSocketMessenger.QueueMessage({socket}, formedMessage);
    }
}

std::string EventSystem::FormMessage(Event& event)
{
    const std::unordered_map<std::string, std::unique_ptr<IProperty>>& props = event.GetProperties();
    nlohmann::json jsonMessage;
    for (const auto& entry : props)
    {
        IProperty* property = entry.second.get();
        PropertyType propertyType = property->GetPropertyType();
        if (propertyType == PropertyType::STRING)
        {
            auto strProperty = dynamic_cast<StringProperty*>(property);
            if (!strProperty)
            {
                continue;
            }
            jsonMessage[entry.first] = strProperty->GetValue();
        }
        else if (propertyType == PropertyType::NUMBER)
        {
            auto numProperty = dynamic_cast<NumberProperty*>(property);
            if (!numProperty)
            {
                continue;
            }
            jsonMessage[entry.first] = numProperty->GetValue();
        }
        else if (propertyType == PropertyType::BOOLEAN)
        {
            auto boolProperty = dynamic_cast<BooleanProperty*>(property);
            if (!boolProperty)
            {
                continue;
            }
            jsonMessage[entry.first] = boolProperty->GetValue();
        }
    }

    return nlohmann::to_string(jsonMessage);
}
