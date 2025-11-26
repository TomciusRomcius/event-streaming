#pragma once

#include <nlohmann/json.hpp>
#include "../eventSystem/eventSystem.h"
#include "../networking/shared/tcpRequest.h"
#include "logging.h"

class ITcpRequestHandler
{
public:
    virtual void Execute(TcpRequest request) = 0;

    virtual ~ITcpRequestHandler() = default;
};

class CreateEventTypeHandler : public ITcpRequestHandler
{
public:
    explicit CreateEventTypeHandler(EventSystem& eventSystem) : m_EventSystem(eventSystem) {}

    void Execute(TcpRequest request) override
    {
        LOG_TRACE("Enetered CreateEventTypeHandler::Execute");
        // TODO: move to_string to macro as even when log level is higher than debug
        // json serialization still happens
        nlohmann::json json = request.body;
        LOG_DEBUG("CreateEventTypeHandler request json: '{}'", nlohmann::to_string(json));
        // TODO: error checking + set max length
        std::string eventTypeName = json["eventType"];
        LOG_DEBUG("Retrieved event type name: '{}'", eventTypeName);
        auto propsArray = json["properties"];

        std::unordered_map<std::string, PropertyType> props;
        for (auto it = propsArray.begin(); it != propsArray.end(); ++it)
        {
            std::string propName = (*it)["key"];
            PropertyType propType = (*it)["type"];
            LOG_DEBUG("Retrieved property key: '{}', type: '{}'", propName, (int) propType);
            props[propName] = propType;
        }

        auto eventType = std::make_unique<EventType>(eventTypeName, std::move(props));
        m_EventSystem.RegisterEventType(std::move(eventType));
    }
private:
    EventSystem& m_EventSystem;
};

class ProduceEventHandler : public ITcpRequestHandler
{
public:
    using jsonIt = nlohmann::json_abi_v3_12_0::detail::iter_impl<nlohmann::json_abi_v3_12_0::json>;

    explicit ProduceEventHandler(EventSystem& eventSystem) : m_EventSystem(eventSystem) {}

    void Execute(TcpRequest request) override
    {
        LOG_TRACE("Entered ProduceEventHandler::Execute");
        // TODO: move to_string to macro as even when log level is higher than debug
        // json serialization still happens
        nlohmann::json json = request.body;
        LOG_DEBUG("ProduceEventHandler request json: '{}'", nlohmann::to_string(json));
        std::string eventTypeName = json["eventType"];
        LOG_DEBUG("Retrieved event type name: '{}'", eventTypeName);
        auto propsArray = json["properties"];
        std::unordered_map<std::string, std::unique_ptr<IProperty>> props;

        EventType* eventType = m_EventSystem.GetEventType(eventTypeName);
        std::unordered_map<std::string, PropertyType>& eventTypeProps = eventType->GetProperties();

        bool error = false;
        for (auto it = propsArray.begin(); it != propsArray.end(); ++it)
        {
            std::string propName = (*it)["key"];
            if (props.find(propName) != props.end())
            {
                LOG_ERROR("Redefining property '{}' for event type '{}'", propName, eventTypeName);
                error = true;
                break;
            }

            auto propertyTypeIt = eventTypeProps.find(propName);
            if (propertyTypeIt == eventTypeProps.end())
            {
                LOG_ERROR("Event type '{}' does not have a property named '{}'!", eventTypeName, propName);
                error = true;
                break;
            }

            PropertyType propType = propertyTypeIt->second;
            IProperty* property = ParseProperty((*it)["value"], propType);
            if (property != nullptr)
            {
                LOG_DEBUG("Retrieved property key: '{}', type: '{}'", propName,
                          static_cast<int>(property->GetPropertyType()));
                props.emplace(propName, property);
            } else
            {
                LOG_ERROR("Failed to parse event type '{}' property '{}'!", eventTypeName, propName);
                error = true;
                break;
            }
        }

        if (!error)
        {
            auto event = Event(eventTypeName, std::move(props));
            m_EventSystem.ProduceEvent(std::move(event));
        }
    }
private:
    static IProperty* ParseProperty(nlohmann::json propValue, PropertyType propType)
    {
        LOG_TRACE("Entered ProduceEventHandler::ParseProperty");
        using value_t = nlohmann::json::value_t;
        if (propType == PropertyType::STRING && propValue.is_string())
        {
            return new StringProperty(propValue);
        } else if (propType == PropertyType::NUMBER && propValue.is_number())
        {
            return new NumberProperty(propValue);
        } else if (propType == PropertyType::BOOLEAN && propValue.is_number())
        {
            return new BooleanProperty(propValue);
        }

        return nullptr;
    }

    EventSystem& m_EventSystem;
};

class SubscribeToEventHandler : public ITcpRequestHandler
{
public:
    explicit SubscribeToEventHandler(EventSystem& eventSystem) : m_EventSystem(eventSystem) {}

    void Execute(TcpRequest request) override
    {
        nlohmann::json json = request.body;
        LOG_TRACE("Entered SubscribeToEventHandler::Execute");
        std::string eventTypeName = json["eventType"];
        LOG_DEBUG("Retrieved event type name: '{}'", eventTypeName);
        GroupId groupId = json["groupId"];
        m_EventSystem.Subscribe(eventTypeName, groupId, request.socket);
    }
private:
    EventSystem& m_EventSystem;
};

class UnsubscribeFromEventHandler : public ITcpRequestHandler
{
public:
    UnsubscribeFromEventHandler(EventSystem& eventSystem) : m_EventSystem(eventSystem) {}

    void Execute(TcpRequest request) override
    {
        LOG_TRACE("Enetered UnsubscribeFromEventHandler::Execute");
        // TODO: move to_string to macro as even when log level is higher than debug
        // json serialization still happens
        nlohmann::json json = request.body;
        LOG_DEBUG("UnsubscribeFromEventHandler request json: '{}'", nlohmann::to_string(json));
        std::string eventTypeName = json["eventType"];
        GroupId groupId = json["groupId"];
        m_EventSystem.Unsubscribe(eventTypeName, groupId, request.socket);
    }
private:
    EventSystem& m_EventSystem;
};
