#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#ifdef __linux__
    #include <arpa/inet.h>
#endif

uint32_t HostToBigEndian32(uint32_t number);
uint32_t BigEndianToHost32(uint32_t number);

std::string GetTcpRequestType(nlohmann::json json);