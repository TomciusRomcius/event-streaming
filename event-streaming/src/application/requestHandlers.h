#pragma once

#include <nlohmann/json.hpp>
#include "../eventSystem/eventSystem.h"

class ITcpRequestHandler
{
public:
    virtual void Execute(nlohmann::json json) = 0;
};

class CreateEventTypeHandler : public ITcpRequestHandler
{
public:
    CreateEventTypeHandler(EventSystem& eventSystem)
        : m_EventSystem(eventSystem)
    { }

    void Execute(nlohmann::json json) override
    {
        std::cout << "Reached CreateEventTypeHandler" << '\n';
        // TODO: error checking + set max length
        std::string eventTypeName = json["eventTypeName"];
        auto propsArray = json["properties"].array();

        std::map<std::string, PropertyType> props;
        for (auto it = propsArray.begin(); it != propsArray.end(); ++it)
        {
            std::string propName = (*it)["name"];
            PropertyType propType = (*it)["type"];
            props[propName] = propType;
        }

        auto eventType = EventType(eventTypeName, props);
        m_EventSystem.RegisterEventType(eventType);
    }
private:
    EventSystem& m_EventSystem;
};
