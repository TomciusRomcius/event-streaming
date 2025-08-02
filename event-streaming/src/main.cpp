#include <map>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <winsock2.h>
#include <set>

enum class PropertyType
{
	STRING,
	INT,
	DECIMAL
};

class EventType
{
public:
	EventType(std::string name, std::map<std::string, PropertyType>& properties)
		: m_Name(name), m_Properties(properties)
	{

	}

	// Checks if all properties are present in the given values map.
	bool ValidateEventValues(const std::map<std::string, void*>& values)
	{
		for (const auto& property : m_Properties)
		{
			auto it = values.find(property.first);
			if (it == values.end())
			{
				return false; // Missing property
			}
		}

		return true;
	}

	inline std::string GetName() const
	{
		return m_Name;
	}

private:
	std::string m_Name;
	std::map<std::string, PropertyType> m_Properties;
};

class ConnectionManager
{
public:
	ConnectionManager(unsigned int serverPort)
	{
		WORD socksVersion = MAKEWORD(2, 2);
		WSADATA wsaData;
		WSAStartup(socksVersion, &wsaData);
		m_ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
		m_ServerAddress.sin_family = AF_INET;
		m_ServerAddress.sin_port = htons(serverPort);
		m_ServerAddress.sin_addr.s_addr = INADDR_ANY;
		if (bind(m_ServerSocket, (sockaddr*)&m_ServerAddress, sizeof(m_ServerAddress)) == -1)
		{
			std::cerr << "Failed to bind server socket" << '\n';
		}
		if (listen(m_ServerSocket, 5) == -1)
		{
			std::cerr << "Failed to setup server socket listener" << '\n';
		}
	}

	void Update()
	{
		AcceptIncomingConnections();
		ReceiveIncomingRequests();
	}

private:
	void AcceptIncomingConnections()
	{
		timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 1000;
		fd_set fdSet;
		FD_ZERO(&fdSet);
		FD_SET(m_ServerSocket, &fdSet);

		int selectResult = select(m_ServerSocket + 1, &fdSet, nullptr, nullptr, &timeout);

		if (selectResult == 0)
		{
			std::cout << "No incoming tcp connections" << '\n';
			return;
		}
		else if (selectResult == -1)
		{
			std::cerr << "An error occured while examining socket file descriptor" << '\n';
			return;
		}

		std::cout << "Incoming tcp connection!" << '\n';
		sockaddr_in clientAddr;
		int clientAddrLen = sizeof(clientAddr);
		int clientSocket = accept(m_ServerSocket, (sockaddr*)&clientAddr, &clientAddrLen);
		if (clientSocket == -1)
		{
			std::cerr << "An error occured while accepting an incoming TCP connection" << '\n';
		}
		m_ClientSockets.insert(clientSocket);
	}

	void ReceiveIncomingRequests()
	{
		std::vector<int> socketRemovalList;

		for (auto& socket : m_ClientSockets)
		{
			timeval timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = 1000;

			fd_set fdSet;
			FD_ZERO(&fdSet);
			FD_SET(socket, &fdSet);

			int selectResult = select(socket + 1, &fdSet, nullptr, nullptr, &timeout);
			if (selectResult > 0)
			{
				char* buffer = (char*)malloc(1024);
				int receivedBufSize = recv(socket, buffer, 1024, 0);
				if (receivedBufSize == 0)
					continue;
				std::cout << "Incoming message: " << std::string(buffer, receivedBufSize);
				free(buffer);
			}
			else if (selectResult == 0)
			{
				socketRemovalList.push_back(socket);
			}
		}

		for (auto& socket : socketRemovalList)
		{
			m_ClientSockets.erase(socket);
		}
	}
private:
	int m_ServerSocket;
	sockaddr_in m_ServerAddress;
	std::set<int> m_ClientSockets;
};

class Event
{
public:
	// TODO: using void* may be dangerous, later implement a type-safe variant
	Event(EventType& type, std::map<std::string, void*>& values)
		: m_EventType(type), m_Values(values)
	{
		if (!m_EventType.ValidateEventValues(values))
		{
			throw std::invalid_argument("Invalid event values provided.");
		}
	}

	inline std::string GetName() const
	{
		return m_EventType.GetName();
	}

private:
	EventType& m_EventType;
	std::map<std::string, void*>& m_Values;
};

class EventStreamer
{
public:
	EventStreamer()
	{

	}

	void RegisterEventType(EventType& eventType)
	{
		if (m_Subscribers.find(eventType.GetName()) != m_Subscribers.end())
		{
			throw std::runtime_error("Event type already registered.");
		}
		m_Subscribers[eventType.GetName()] = {};
	}

	void Subscribe(EventType& eventType, std::string ipAddress)
	{
		auto it = m_Subscribers.find(eventType.GetName());
		if (it == m_Subscribers.end())
		{
			throw std::invalid_argument("Event type has not been registered");
		}
		else
		{
			it->second.push_back(ipAddress);
		}
	}

	void Unsubscribe(EventType& eventType, std::string ipAddress)
	{
	}

	void ProduceEvent(const Event& event) const
	{
		// TODO: store

		Publish(event);
	}

private:
	void Publish(const Event& event) const
	{
		auto it = m_Subscribers.find(event.GetName());
		if (it == m_Subscribers.end())
		{
			throw std::runtime_error("Event type has not been registered!");
		}

		for (std::string ipAddress : it->second)
		{
			std::cout << "Sending event to: " << ipAddress << '\n';
		}
	}

private:
	// event type name -> ipAddresses
	// TODO: linked list may be better for performance on higher loads
	std::map<std::string, std::vector<std::string>> m_Subscribers;
};

class Application
{
public:
	Application()
		: m_ConnectionManager(9000)
	{
	}

	~Application()
	{
		WSACleanup();
	}

	void Start()
	{
		while (1)
		{
			std::cout << "Looping" << '\n';
			m_ConnectionManager.Update();
			_sleep(100); // Release current thread so we don't overwork the thread for now
		}
	}

private:
	ConnectionManager m_ConnectionManager;
};

int main()
{
	//auto streamer = EventStreamer();
	//std::map<std::string, PropertyType> properties = {
	//	{ "key1", PropertyType::INT },
	//	{ "key2", PropertyType::DECIMAL }
	//};

	//auto eventType = EventType("TestEvent", properties);

	//streamer.RegisterEventType(eventType);
	//streamer.Subscribe(eventType, "127.0.0.1");

	//std::map<std::string, void*> values = {
	//	{ "key1", (void*)new int(42) },
	//	{ "key2", (void*)new double(42.2) },
	//};
	//auto testEvent = Event(
	//	eventType,
	//	values
	//);
	//streamer.ProduceEvent(testEvent);

	Application app;
	app.Start();
	return 0;
}
