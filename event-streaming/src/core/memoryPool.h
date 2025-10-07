#pragma once
#include <mutex>
#include <set>

#define MAX_CHUNK_SIZE 4098

class MemoryChunk
{
	friend class MemoryChunkUser;
public:
	MemoryChunk(uint16_t chunkSize)
		: m_ChunkSize(chunkSize)
	{
		m_Buffer = malloc(chunkSize);
	}

	~MemoryChunk()
	{
		free(m_Buffer);
	}

	inline void* GetBuffer() const
	{
		return m_Buffer;
	}

	inline uint16_t GetSize() const { return m_ChunkSize; }
private:
	bool m_IsInUse = false;
	uint16_t m_ChunkSize;
	void* m_Buffer;
};

class MemoryChunkUser
{
public:
	MemoryChunkUser(MemoryChunk& memoryChunk)
		: m_MemoryChunk(memoryChunk)
	{
		LOG_DEBUG("Created MemoryChunkUser with memory chunk size: '{}'", memoryChunk.GetSize());
		if (memoryChunk.m_IsInUse)
		{
			LOG_WARN("Trying to create a MemoryChunkUser for a memory chunk that is in use!");
		}
		m_MemoryChunk.m_IsInUse = true;
	}

	~MemoryChunkUser()
	{
		m_MemoryChunk.m_IsInUse = false;
	}

	inline void* GetBuffer() const
	{
		return m_MemoryChunk.GetBuffer();
	}

private:
	MemoryChunk& m_MemoryChunk;
};

inline auto MemoryChunkComp = [](const MemoryChunk& mc1, const MemoryChunk& mc2) { return mc1.GetSize() < mc2.GetSize(); };
typedef std::set<MemoryChunk, decltype(MemoryChunkComp)> MemoryChunkSet;

class MemoryPool
{
public:
	MemoryPool(int maxPoolCount)
		: m_MaxPoolCount(maxPoolCount), m_MemoryChunks(MemoryChunkComp)
	{

	}

	std::optional<MemoryChunkUser> GetMemoryChunk(int16_t chunkSize)
	{
		LOG_TRACE("Entered GetMemoryChunk");
		LOG_DEBUG("Requesting memory chunk of size {}", chunkSize);
		if (chunkSize > MAX_CHUNK_SIZE)
		{
			LOG_WARN("Trying to get a memory chunk with a size, that exceeds MAX_CHUNK_SIZE!");
			return std::nullopt;
		}

		std::lock_guard<std::mutex> lock(m_Mutex);

		for (auto it = m_MemoryChunks.begin(); it != m_MemoryChunks.end(); ++it)
		{
			if (it->GetSize() >= chunkSize)
			{
				// Its okay to remove const cast, since size won't be changed. TODO: should find a better way though
				auto& memoryChunk = const_cast<MemoryChunk&>(*it);
				return MemoryChunkUser(memoryChunk);
			}
		}

		// No memory chunk with correct size

		LOG_DEBUG("Creating memory chunk with size: {}", chunkSize);
		m_MemoryChunks.emplace(chunkSize);
		if (m_MemoryChunks.size() > m_MaxPoolCount)
		{
			auto frontIt = m_MemoryChunks.begin();
			m_MemoryChunks.erase(frontIt);
		}

		// Its okay to remove const cast, since size won't be changed. TODO: should find a better way though
		auto& memoryChunk = const_cast<MemoryChunk&>(*m_MemoryChunks.rbegin());
		return MemoryChunkUser(memoryChunk);
	}
private:
	MemoryChunkSet m_MemoryChunks;
	int m_MaxPoolCount;
	std::mutex m_Mutex;
};
