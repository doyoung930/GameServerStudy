#pragma once
#include"header.h"




class EXP_OVER {
public:
	WSAOVERLAPPED _wsa_over;
	int _s_id;
	WSABUF _wsa_buf;
	char _send_msg[BUFSIZE];

	
public:
	EXP_OVER(char s_id, char packet_type,char size, char* addr) :_s_id(s_id)
	{
		ZeroMemory(&_wsa_over, sizeof(_wsa_over));
		addr[0] = size;
		addr[1] = packet_type;
		addr[2] = s_id;
		_wsa_buf.buf = addr;
		_wsa_buf.len = size;

		
		cout <<"exp_over »ý¼º" <<"size:" <<(int)addr[0]<< 
			"type:" << (int)addr[1]<<"id:" << (int)addr[2] << endl;

	}
	~EXP_OVER()
	{

	}

	
};
class SESSION {
private:

	int _id;
	WSABUF _recv_wsabuf;
	WSABUF _send_wsabuf;
	WSAOVERLAPPED _recv_over;
	SOCKET _socket;
	
public:
	char _recv_buf[BUFSIZE];
	char _send_buf[BUFSIZE];
	bool DetroySession = false;
	MOVEPACKET m_player_move;
	STARTPACKET m_player_start;
	ENDPACKET m_player_end;
public:
	SESSION()
	{
		cout << "Unexpected Constructor Call Error." << endl;
		exit(-1);
	}
	SESSION(int id, SOCKET s);
	~SESSION()
	{
		
		closesocket(_socket);
	}
	void do_recv();

	void do_send(char s_id, char packet_type, char size, char* addr);

	static void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD recv_flag);
	



	static void CALLBACK recv_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED recv_over, DWORD recv_flag);
	
};