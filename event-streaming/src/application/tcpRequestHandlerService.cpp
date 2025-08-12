#include "tcpRequestHandlerService.h"

void TcpRequestHandlerService::RegisterStrategy(const std::string& requestType, std::unique_ptr<ITcpRequestHandler>&& handler)
{
    m_StrategyMap[requestType] = std::move(handler);
}

bool TcpRequestHandlerService::TryExecuteStrategy(std::string requestType, TcpRequest&& request)
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
