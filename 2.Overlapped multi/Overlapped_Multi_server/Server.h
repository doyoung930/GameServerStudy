#pragma once
#include "header.h"

class SESSION; // SESSION Ŭ������ ������ ��� ���Ͽ� ���ǵǾ� �ִٰ� �����մϴ�.

class Server
{
private:
	static Server* m_instance; // �ν��Ͻ� ������
	static bool m_isLooping; // ���� ���� ����

public:
	// �̱��� �ν��Ͻ��� �������� �Լ�
	static Server* GetInstance()
	{
		if (!m_instance)
			m_instance = new Server;
		return m_instance;
	}

	// �̱��� �ν��Ͻ��� �����ϴ� �Լ�
	static void DestroyInstance()
	{
		if (m_instance)
		{
			delete m_instance;
			m_instance = NULL;
		}
	}

public:
	Server();
	~Server();

private:
	WSADATA m_wsaData; // Winsock ������
	SOCKET m_serverSocket; // ���� ����
	SOCKADDR_IN m_serverAddress; // ���� �ּ� ����
	INT m_addressSize; // �ּ� ���� ũ��

	SOCKET m_clientSocket; // ���� ����� Ŭ���̾�Ʈ ����
	WSABUF m_recvWsaBuf; // ���� WSABUF
	WSABUF m_sendWsaBuf; // �۽� WSABUF

	WSAOVERLAPPED m_sendOverlapped; // �۽� ������ ����ü
	WSAOVERLAPPED m_recvOverlapped; // ���� ������ ����ü

	DWORD m_bytesReceived; // ���ŵ� ����Ʈ ��
	DWORD m_recvFlags; // ���� �÷���
	DWORD m_bytesSent; // �۽ŵ� ����Ʈ ��
	bool m_isShuttingDown = false; // ���� ���� ��û ����

	MOVEPACKET m_playerMovePacket; // �÷��̾� �̵� ��Ŷ
	STARTPACKET m_startPacket; // ���� ��Ŷ
	ENDPACKET m_endPacket; // ���� ��Ŷ

public:
	// Ŭ���̾�Ʈ ���� ��� (int�� Ŭ���̾�Ʈ ID �Ǵ� ���� �� Ű ��)
	std::unordered_map<int, SESSION*> m_clientSessions;

private:
	// ���� �ּ� ������ �ʱ�ȭ�ϴ� �Լ�
	void InitializeServerAddress();
	// �̵� ��Ŷ�� �����ϴ� �Լ�
	// Ŭ���̾�Ʈ�κ��� �̵� ���� ��Ŷ�� �����ϴ� �Լ� 
	void ReceiveMovePacket();
	// Ŭ���̾�Ʈ���� �̵� ���� ��Ŷ�� �۽��ϴ� �Լ� 
	void SendMovePacket();

public:
	// ������ �ʱ�ȭ�ϴ� �Լ� 
	bool Initialize();
	// ���� ���� ������ �����Ͽ� Ŭ���̾�Ʈ ���� �� ������ ó���� �����ϴ� �Լ�
	bool Run();
	// (�ּ�: �� �Լ��� � key �Է¿� ���� ���� ��ü�� ������ �����ϴ� ������, 
	// �ƴϸ� Ư�� Ŭ���̾�Ʈ�� �̵��� ó���ϴ� ������ ��Ȯ���� �ʾ� ���� �Լ��� ����)
	// �÷��̾� �̵� �Ǵ� ���� ���� ���� ������ �����ϴ� �Լ�
	void Move(int key);

	// Winsock ���� �޽����� ����ϴ� ���� �Լ�
	void DisplayErrorMessage(const char* msg)
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf, 0, NULL);
		printf("[%s] %s", msg, (char*)lpMsgBuf);
		LocalFree(lpMsgBuf);
	}
};

// ���� ��� ������ Ŭ���� ���� �ܺο��� �ʱ�ȭ�ؾ� �մϴ�.
// (�� .h ������ include �ϴ� .cpp ���Ͽ� �߰��Ǿ�� �մϴ�.)
// Server* Server::m_instance = NULL;
// bool Server::m_isLooping = false;