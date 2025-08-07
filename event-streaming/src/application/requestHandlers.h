#pragma once

#include <nlohmann/json.hpp>
#include "logging.h"
#include "../eventSystem/eventSystem.h"

class ITcpRequestHandler
{
public:
    virtual void Execute(nlohmann::json json) = 0;
    virtual ~ITcpRequestHandler() = default;
};

class CreateEventTypeHandler : public ITcpRequestHandler
{
public:
    explicit CreateEventTypeHandler(EventSystem& eventSystem)
        : m_EventSystem(eventSystem)
    { }

    void Execute(nlohmann::json json) override
    {
        LOG_TRACE("Enetered CreateEventTypeHandler::Execute");
        // TODO: move to_string to macro as even when log level is higher than debug
        // json serialization still happens
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
            LOG_DEBUG("Retrieved property key: '{}', type: '{}'", propName, (int)propType);
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

    explicit ProduceEventHandler(EventSystem& eventSystem)
        : m_EventSystem(eventSystem)
    { }

    void Execute(nlohmann::json json) override
    {
        LOG_TRACE("Enetered ProduceEventHandler::Execute");
        // TODO: move to_string to macro as even when log level is higher than debug
        // json serialization still happens
        LOG_DEBUG("CreateEventTypeHandler request json: '{}'", nlohmann::to_string(json));
        std::string eventTypeName = json["eventType"];
        LOG_DEBUG("Retrieved event type name: '{}'", eventTypeName);
        auto propsArray = json["properties"];
        std::unordered_map<std::string, std::unique_ptr<IProperty>> props;
        for (auto it = propsArray.begin(); it != propsArray.end(); ++it)
        {
            std::string propName = (*it)["key"];
            IProperty* property = ParseProperty(*it);
            LOG_DEBUG("Retrieved property key: '{}', type: '{}'", propName, (int)property->GetPropertyType());
            if (property != nullptr)
            {
                props[propName] = std::unique_ptr<IProperty>(property);
            }
        }

        auto event = Event(eventTypeName, std::move(props));
        m_EventSystem.ProduceEvent(std::move(event));
    }

private:
    IProperty* ParseProperty(nlohmann::json propJson)
    {
        LOG_TRACE("Enetered ProduceEventHandler::ParseProperty");
        using value_t = nlohmann::json::value_t;
        auto propType = propJson["value"].type();
        switch (propType)
        {
        case value_t::string:
            return new StringProperty(propJson["value"]);
        default:
            return nullptr;
        }
    }
    EventSystem& m_EventSystem;
};