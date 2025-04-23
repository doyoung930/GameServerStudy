#include "Server.h"
#include "Session.h"

Server* Server::m_instance = NULL;

Server::Server() : m_recvFlags(0)
{
}

Server::~Server()
{
	for (auto& pair : m_clientSessions)
	{
		delete pair.second;
	}
	WSACleanup();
}

void Server::InitializeServerAddress()
{
	ZeroMemory(&m_serverAddress, sizeof(m_serverAddress));
	m_serverAddress.sin_family = AF_INET;
	m_serverAddress.sin_port = htons(SERVER_PORT);
	m_serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
}

void Server::Move(int key)
{
	switch (static_cast<MOVETYPE>(m_clientSessions[key]->m_player_move.move_type))
	{
	case MOVETYPE::LEFT:
		m_clientSessions[key]->m_player_move.x -= 1;
		if (m_clientSessions[key]->m_player_move.x < 0)
			m_clientSessions[key]->m_player_move.x = 0;
		break;
	case MOVETYPE::RIGHT:
		m_clientSessions[key]->m_player_move.x += 1;
		if (m_clientSessions[key]->m_player_move.x > 7)
			m_clientSessions[key]->m_player_move.x = 7;
		break;
	case MOVETYPE::UP:
		m_clientSessions[key]->m_player_move.y -= 1;
		if (m_clientSessions[key]->m_player_move.y < 0)
			m_clientSessions[key]->m_player_move.y = 0;
		break;
	case MOVETYPE::DOWN:
		m_clientSessions[key]->m_player_move.y += 1;
		if (m_clientSessions[key]->m_player_move.y > 7)
			m_clientSessions[key]->m_player_move.y = 7;
		break;
	}
}

bool Server::Initialize()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
		return false;

	m_serverSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);

	InitializeServerAddress();

	int result = bind(m_serverSocket, reinterpret_cast<sockaddr*>(&m_serverAddress), sizeof(m_serverAddress));
	if (result == SOCKET_ERROR)
	{
		DisplayErrorMessage("bind()");
		return false;
	}

	result = listen(m_serverSocket, SOMAXCONN);
	if (result == SOCKET_ERROR)
	{
		DisplayErrorMessage("listen()");
		return false;
	}

	m_addressSize = sizeof(m_serverAddress);

	return true;
}

bool Server::Run()
{
	for (int i = 1; i <= 10; ++i)
	{
		SOCKET clientSocket = WSAAccept(m_serverSocket, reinterpret_cast<sockaddr*>(&m_serverAddress), &m_addressSize, 0, 0);

		if (m_clientSocket == INVALID_SOCKET) {
			DisplayErrorMessage("accept()");
		}
		else
			std::cout << "클라이언트 연결 성공" << std::endl;

		int tcpOption = 1;
		setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&tcpOption), sizeof(tcpOption));

		SESSION* session = new SESSION(i, clientSocket);
		m_clientSessions.emplace(i, session);

		m_clientSessions[i]->do_send((char)i,
			m_clientSessions[i]->m_player_start.packet_type,
			m_clientSessions[i]->m_player_start.size,
			(char*)&m_clientSessions[i]->m_player_start);

		m_clientSessions[i]->do_recv();
	}

	while (!m_isShuttingDown)
		SleepEx(100, true);

	return false;
}
