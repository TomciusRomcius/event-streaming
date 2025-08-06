#pragma once

#include <nlohmann/json.hpp>
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
        std::cout << "Reached CreateEventTypeHandler" << '\n';
        // TODO: error checking + set max length
        std::string eventTypeName = json["eventType"];
        auto propsArray = json["properties"];
 
        std::unordered_map<std::string, PropertyType> props;
        for (auto it = propsArray.begin(); it != propsArray.end(); ++it)
        {
            std::string propName = (*it)["key"];
            PropertyType propType = (*it)["type"];
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
        std::string eventTypeName = json["eventType"];
        auto propsArray = json["properties"];
        std::unordered_map<std::string, std::unique_ptr<IProperty>> props;
        for (auto it = propsArray.begin(); it != propsArray.end(); ++it)
        {
            std::string propName = (*it)["key"];
            IProperty* property = ParseProperty(*it);
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