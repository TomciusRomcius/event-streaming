#ifdef _WIN32

#include <winsock2.h>

class TcpSocketMessenger
{
public:
	TcpSocketMessenger(TcpConnectionPool& tcpConnectionPool)
		: m_TcpConnectionPool(tcpConnectionPool)
	{

	}

	bool SendRequest(std::vector<unsigned int>& targetSockets, std::string message)
	{
		for (auto socket : targetSockets)
		{
			if (!m_TcpConnectionPool.clientSockets.contains(socket))
			{
				throw std::runtime_error("Trying to send a message to a socket that does not exist!");
			}

			const char* messageBuffer = message.c_str();
			if (send(socket, messageBuffer, message.length(), 0) == -1)\
			{
				std::cerr << "Encountered an error while sending a message to a socket" << '\n';
			}
		}
	}

private:
	TcpConnectionPool& m_TcpConnectionPool;
};

class TcpMessageReceiver
{
public:
	TcpMessageReceiver(TcpConnectionPool& tcpConnectionPool)
		: m_TcpConnectionPool(tcpConnectionPool)
	{

	}

	void TryReceiveMessage()
	{

	}
private:
	TcpConnectionPool& m_TcpConnectionPool;
};

class TcpSocketConnectionManager
{
public:
	TcpSocketConnectionManager(TcpConnectionPool& tcpConnectionPool, int serverPort)
		: m_TcpConnectionPool(tcpConnectionPool), m_ServerPort(serverPort)
	{

	}

	void InitializeServerSocket()
	{
		WORD socksVersion = MAKEWORD(2, 2);
		WSADATA wsaData;
		if (WSAStartup(socksVersion, &wsaData) == -1)
		{
			throw std::runtime_error("Failed to initialize winsock2");
		}

		m_ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
		m_ServerAddress.sin_family = AF_INET;
		m_ServerAddress.sin_port = htons(m_ServerPort);
		m_ServerAddress.sin_addr.s_addr = INADDR_ANY;
		if (bind(m_ServerSocket, (sockaddr*)&m_ServerAddress, sizeof(m_ServerAddress)) == -1)
		{
			throw std::runtime_error("Failed to bind server socket");
		}
		if (listen(m_ServerSocket, 5) == -1)
		{
			throw std::runtime_error("Failed to setup server socket listener");
		}
	}

	void TryAcceptIncomingConnection()
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
			return;
		}

		if (selectResult == -1)
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
		m_TcpConnectionPool.clientSockets.insert(clientSocket);
	}

	void TerminateConnection()
	{

	}

private:
	TcpConnectionPool& m_TcpConnectionPool;
	unsigned int m_ServerPort;
	unsigned int m_ServerSocket;
	sockaddr_in m_ServerAddress;
};
#endif // _WIN32