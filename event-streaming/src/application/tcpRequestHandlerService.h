#include <nlohmann/json.hpp>
#include "../eventSystem/eventSystem.h"
#include "requestHandlers.h"


class TcpRequestHandlerService
{
public:
    void RegisterStrategy(std::string requestType, ITcpRequestHandler* handler)
    {
        m_StrategyMap[requestType] = std::unique_ptr<ITcpRequestHandler>(handler);
    }

    bool TryExecuteStrategy(std::string requestType, nlohmann::json requestJson)
    {
        auto strategyIt = m_StrategyMap.find(requestType);
        if (strategyIt != m_StrategyMap.end())
        {
            (*strategyIt).second->Execute(requestJson);
            return true;
        }
        return false;
    }

private:
    // Maps TCP request type to a strategy that handles the request
    std::unordered_map<std::string, std::unique_ptr<ITcpRequestHandler>> m_StrategyMap;
};