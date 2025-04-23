#pragma once
#include"header.h"


class Server
{
private:
	static Server* m_pInst;
	
public:
	static bool m_bLoop;
	static Server* GetInst()
	{
		if (!m_pInst)
			m_pInst = new Server;
		return m_pInst;
	}

	static void DestroyInst()
	{
		if (m_pInst)
		{
			delete m_pInst;
			m_pInst = NULL;
		}
	}
public:
	Server();
	~Server();
private:
	WSADATA m_WsaData;
	SOCKET m_s_socket;
	SOCKADDR_IN m_server_addr;

	MOVEPACKET player_move;
	STARTPACKET m_start_packet;
	ENDPACKET m_end_packet;

	WSAOVERLAPPED m_send_over;
	WSAOVERLAPPED m_recv_over;

	WSABUF m_rWsaBuf;
	WSABUF m_sWsaBuf;
	DWORD m_recv_byte;
	DWORD m_recv_flag;
	DWORD sent_byte;
	char SERVER_ADDR[256] = { 0, };
	//const char* SERVER_ADDR = "127.0.0.1";
	char m_temp_buf[BUFSIZE] = { 0, };
	int m_tcp_option;
	int id;
private:
	void InitServerAddr();
public:
	bool Init();
	bool ALLRecv();
	void err_display(const char* msg)
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf, 0, NULL);
		printf("[%s] ", msg);
		std::wcout << L"¿¡·¯ " << (LPTSTR)lpMsgBuf << std::endl;
		LocalFree(lpMsgBuf);
	}
	bool SendMove();
	
	bool SendStart(int);
	bool SendEnd();
	static void CALLBACK recv_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED recv_over, DWORD flag);
	static void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag);
public:
	MOVEPACKET GetMove()
	{
		return player_move;
	}
	void SetMove(char type)
	{
		player_move.move_type = type;
		
	}
};

