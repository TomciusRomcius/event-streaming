#include <nlohmann/json.hpp>
#include "logging.h"

uint32_t HostToBigEndian32(uint32_t number)
{
	return htonl(number);
}

uint32_t BigEndianToHost32(uint32_t number)
{
	return ntohl(number);
}

std::string GetTcpRequestType(nlohmann::json json)
{
	LOG_TRACE("Entered GetTcpRequestType");
	if (!json.contains("type"))
	{
		LOG_ERROR("Failed to get TCP request type");
		return "err";
	}

	if (!json["type"].is_string())
	{
		LOG_ERROR("Failed to get TCP request type");
		return "err";
	}

	return json["type"];
}