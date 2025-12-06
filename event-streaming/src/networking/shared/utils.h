#pragma once
#include <cstdint>
#include <string>
#include "core/memoryPool.h"

MemoryChunkUser FormTcpMessage(MemoryPool& memoryPool, const std::string& message, uint32_t* bufferSize);
