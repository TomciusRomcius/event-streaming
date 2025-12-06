#include "application/utils.h"
#include "tcpSocketMessenger.h"

MemoryChunkUser FormTcpMessage(MemoryPool& memoryPool, const std::string& message, uint32_t* bufferSize)
{
    LOG_TRACE("Entered FormTcpMessage");
    uint32_t messageSize = message.length();
    // 4 additional bytes are allocated for a uint32 to specify message size and
    // establish TCP message boundaries
    *bufferSize = 4 + messageSize;
    std::optional<MemoryChunkUser> memoryChunk = memoryPool.GetMemoryChunk(*bufferSize);
    if (!memoryChunk.has_value())
    {
        throw std::runtime_error("Memory chunk allocation failed");
    }
    void* buffer = memoryChunk->GetBuffer();
    auto* sizePointer = static_cast<uint32_t*>(buffer);
    *sizePointer = HostToBigEndian32(messageSize);
    void* messagePointer = reinterpret_cast<void*>(sizePointer + 1);
    memcpy(messagePointer, message.c_str(), messageSize);
    LOG_DEBUG("TCP message size: {}", *bufferSize);
    LOG_DEBUG("User message size: {}", messageSize);
    return std::move(memoryChunk.value());
}
