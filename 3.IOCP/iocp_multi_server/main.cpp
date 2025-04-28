#include <iostream>
#include <array>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>

#include "..\protocol.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
using namespace std;
using namespace chrono;

constexpr int MAX_USER = 10000;

enum COMP_TYPE { ACCEPT, RECV, SEND };

class EXP_OVER {
public:
	WSAOVERLAPPED over;
	WSABUF wsaBuf;
	char sendBuf[BUF_SIZE];
	COMP_TYPE compType;

	EXP_OVER() {
		wsaBuf.len = BUF_SIZE;
		wsaBuf.buf = sendBuf;
		compType = RECV;
		ZeroMemory(&over, sizeof(over));
	}

	EXP_OVER(char* packet){
		wsaBuf.len = packet[0];
		wsaBuf.buf = sendBuf;
		ZeroMemory(&over, sizeof(over));
		compType = SEND;
		memcpy(sendBuf, packet, packet[0]);
	}
};

enum S_STATE { ST_FREE, ST_ALLOC, ST_INGAME };

class SESSION {

private:
	EXP_OVER	recvOver;
public:
	mutex		sLock;
	S_STATE		state;
	int			id;
	SOCKET		socket;
	short		x, y;
	char		name[NAME_SIZE];
	int			prevRemain;
	int			lastMoveTime;

// fucntion
	SESSION() : socket(0)
	{
		id = -1;
		x = y = 0;
		name[0] = 0;
		state = ST_FREE;
		prevRemain = 0;
	}

	~SESSION() {}

	void doRecv() {
		DWORD recvFlag = 0;
		memset(&recvOver.over, 0, sizeof(recvOver.over));
		recvOver.wsaBuf.len = BUF_SIZE - prevRemain;
		recvOver.wsaBuf.buf = recvOver.sendBuf + prevRemain;
		WSARecv(socket, &recvOver.wsaBuf, 1, 0, &recvFlag, &recvOver.over, 0);
	}

	const void doSend(void* packet) const
	{
		EXP_OVER* data = new EXP_OVER{ reinterpret_cast<char*>(packet) };
		WSASend(socket, &data->wsaBuf, 1, 0, 0, &data->over, 0);
	}

	void sendLoginInfoPacket() {
		SC_LOGIN_INFO_PACKET p;
		p.id = id;
		p.size = sizeof(SC_LOGIN_INFO_PACKET);
		p.type = SC_LOGIN_INFO;
		p.x = x;
		p.y = y;

		doSend(&p);
	}

	void SendMovePacket(int c_id);
};

array <SESSION, MAX_USER> clients;
SOCKET g_s_socket, g_c_socket;
EXP_OVER g_a_over;

void SESSION::SendMovePacket(int c_id) {
	SC_MOVE_PLAYER_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_MOVE_PLAYER_PACKET);
	p.type = SC_MOVE_PLAYER;
	p.x = clients[c_id].x;
	p.y = clients[c_id].y;

	p.move_time = clients[c_id].lastMoveTime;
	doSend(&p);
}


int main() {

}