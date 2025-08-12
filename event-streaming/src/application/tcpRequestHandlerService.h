#pragma once

#include <nlohmann/json.hpp>
#include "../eventSystem/eventSystem.h"
#include "requestHandlers.h"

class TcpRequestHandlerService
{
public:
    void RegisterStrategy(const std::string& requestType, std::unique_ptr<ITcpRequestHandler>&& handler);
    bool TryExecuteStrategy(std::string requestType, TcpRequest&& request);
private:
    // Maps TCP request type to a strategy that handles the request
    std::unordered_map<std::string, std::unique_ptr<ITcpRequestHandler>> m_StrategyMap;
};