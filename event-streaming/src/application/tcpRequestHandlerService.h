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

    bool TryExecuteStrategy(std::string requestType, TcpRequest&& request)
    {
        LOG_TRACE("Entered TcpRequestHandlerService::TryExecuteStrategy");
        LOG_DEBUG("Trying to execute strategy for request type: '{}'", requestType);
        auto strategyIt = m_StrategyMap.find(requestType);
        if (strategyIt != m_StrategyMap.end())
        {
            LOG_DEBUG("Strategy for '{}' found!", requestType);
            (*strategyIt).second->Execute(std::move(request));
            return true;
        }

        LOG_WARN("Strategy for '{}' not found!", requestType);
        return false;
    }

private:
    // Maps TCP request type to a strategy that handles the request
    std::unordered_map<std::string, std::unique_ptr<ITcpRequestHandler>> m_StrategyMap;
};