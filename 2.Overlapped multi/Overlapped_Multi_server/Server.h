#pragma once
#include "header.h"

class SESSION; // SESSION 클래스는 별도의 헤더 파일에 정의되어 있다고 가정합니다.

class Server
{
private:
	static Server* m_instance; // 인스턴스 포인터
	static bool m_isLooping; // 루프 실행 상태

public:
	// 싱글톤 인스턴스를 가져오는 함수
	static Server* GetInstance()
	{
		if (!m_instance)
			m_instance = new Server;
		return m_instance;
	}

	// 싱글톤 인스턴스를 제거하는 함수
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
	WSADATA m_wsaData; // Winsock 데이터
	SOCKET m_serverSocket; // 리슨 소켓
	SOCKADDR_IN m_serverAddress; // 서버 주소 정보
	INT m_addressSize; // 주소 정보 크기

	SOCKET m_clientSocket; // 새로 연결된 클라이언트 소켓
	WSABUF m_recvWsaBuf; // 수신 WSABUF
	WSABUF m_sendWsaBuf; // 송신 WSABUF

	WSAOVERLAPPED m_sendOverlapped; // 송신 오버랩 구조체
	WSAOVERLAPPED m_recvOverlapped; // 수신 오버랩 구조체

	DWORD m_bytesReceived; // 수신된 바이트 수
	DWORD m_recvFlags; // 수신 플래그
	DWORD m_bytesSent; // 송신된 바이트 수
	bool m_isShuttingDown = false; // 서버 종료 요청 상태

	MOVEPACKET m_playerMovePacket; // 플레이어 이동 패킷
	STARTPACKET m_startPacket; // 시작 패킷
	ENDPACKET m_endPacket; // 종료 패킷

public:
	// 클라이언트 세션 목록 (int는 클라이언트 ID 또는 소켓 등 키 값)
	std::unordered_map<int, SESSION*> m_clientSessions;

private:
	// 서버 주소 정보를 초기화하는 함수
	void InitializeServerAddress();
	// 이동 패킷을 수신하는 함수
	// 클라이언트로부터 이동 관련 패킷을 수신하는 함수 
	void ReceiveMovePacket();
	// 클라이언트에게 이동 관련 패킷을 송신하는 함수 
	void SendMovePacket();

public:
	// 서버를 초기화하는 함수 
	bool Initialize();
	// 서버 메인 루프를 실행하여 클라이언트 연결 및 데이터 처리를 수행하는 함수
	bool Run();
	// (주석: 이 함수는 어떤 key 입력에 따라 서버 자체의 동작을 제어하는 것인지, 
	// 아니면 특정 클라이언트의 이동을 처리하는 것인지 명확하지 않아 원본 함수명 유지)
	// 플레이어 이동 또는 서버 제어 관련 로직을 수행하는 함수
	void Move(int key);

	// Winsock 오류 메시지를 출력하는 헬퍼 함수
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

// 정적 멤버 변수는 클래스 정의 외부에서 초기화해야 합니다.
// (이 .h 파일을 include 하는 .cpp 파일에 추가되어야 합니다.)
// Server* Server::m_instance = NULL;
// bool Server::m_isLooping = false;