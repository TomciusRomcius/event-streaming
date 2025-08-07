#pragma once

#include <nlohmann/json.hpp>

struct TcpRequest
{
    TcpRequest(unsigned int s, nlohmann::json b)
        : socket(s), body(b) 
    {}
    unsigned int socket;
    nlohmann::json body;
};