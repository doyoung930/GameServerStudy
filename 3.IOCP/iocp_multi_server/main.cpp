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

	void DoRecv() {
		DWORD recvFlag = 0;
		memset(&recvOver.over, 0, sizeof(recvOver.over));
		recvOver.wsaBuf.len = BUF_SIZE - prevRemain;
		recvOver.wsaBuf.buf = recvOver.sendBuf + prevRemain;
		WSARecv(socket, &recvOver.wsaBuf, 1, 0, &recvFlag, &recvOver.over, 0);
	}

	const void DoSend(void* packet) const
	{
		EXP_OVER* data = new EXP_OVER{ reinterpret_cast<char*>(packet) };
		WSASend(socket, &data->wsaBuf, 1, 0, 0, &data->over, 0);
	}

	void SendLoginInfoPacket() {
		SC_LOGIN_INFO_PACKET p;
		p.id = id;
		p.size = sizeof(SC_LOGIN_INFO_PACKET);
		p.type = SC_LOGIN_INFO;
		p.x = x;
		p.y = y;

		DoSend(&p);
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
	DoSend(&p);
}

int GetNewClientId() {
	for (int i = 0; i < MAX_USER; ++i) {
		lock_guard<mutex> ll{ clients[i].sLock };
		if (clients[i].state == ST_FREE)
			return i;
	}

	return -1;
}

void ProcessPacket(int c_id, char* packet) {
	switch (packet[1]) {
	case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		strcpy_s(clients[c_id].name, p->name);
		clients[c_id].SendLoginInfoPacket();
		{
			lock_guard<mutex> ll{ clients[c_id].sLock };
			clients[c_id].state = ST_INGAME;
		}

		for (auto& pl : clients) {
			{
				lock_guard<mutex> ll(pl.sLock);
				if (ST_INGAME != pl.state) continue;
			}
			if (pl.id = c_id) continue;
			SC_ADD_PLAYER_PACKET addPacket;
			addPacket.id = pl.id;
			strcpy_s(addPacket.name, pl.name);
			addPacket.size = sizeof(addPacket);
			addPacket.type = SC_ADD_PLAYER;
			addPacket.x = clients[c_id].x;
			addPacket.y = clients[c_id].y;
			pl.DoSend(&addPacket);
		}

		for (auto& pl : clients) {
			{
				lock_guard<mutex> ll(pl.sLock);
				if (ST_INGAME != pl.state) continue;
			}
			if (pl.id = c_id) continue;
			SC_ADD_PLAYER_PACKET addPacket;
			addPacket.id = pl.id;
			strcpy_s(addPacket.name, pl.name);
			addPacket.size = sizeof(addPacket);
			addPacket.type = SC_ADD_PLAYER;
			addPacket.x = pl.x;
			addPacket.y = pl.y;
			clients[c_id].DoSend(&addPacket);
		}
		break;
	}

	case CS_MOVE: {

	}


	}
}

void Disconnect(int c_id) {
	for (auto& pl : clients) {
		{
			lock_guard<mutex> ll(pl.sLock);
			if (ST_INGAME != pl.state) continue;
		}
		if (pl.id == c_id) continue;

		SC_REMOVE_PLAYER_PACKET p;

		p.id = c_id;
		p.size = sizeof(p);
		p.type = SC_REMOVE_PLAYER;
		pl.DoSend(&p);
	}

	closesocket(clients[c_id].socket);

	lock_guard<mutex> ll(clients[c_id].sLock);
	clients[c_id].state = ST_FREE;

}

void WorkerThread(HANDLE hIocp) {
	while (true) {
		DWORD numBytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(hIocp, &numBytes, &key, &over, INFINITE);
		EXP_OVER* exOver = reinterpret_cast<EXP_OVER*>(over);

		if (FALSE == ret) {
			if (exOver->compType == ACCEPT)
				cout << "Accept Error";
			else {
				cout << "GQCS Error on client[" << key << "]\n";

				Disconnect(static_cast<int>(key));

				if (exOver->compType == SEND)
					delete exOver;
				continue;
			}
		}

		switch (exOver->compType) {
		case ACCEPT: {
			int client_id = GetNewClientId();
			if (client_id != -1) {
				{
					lock_guard<mutex> ll(clients[client_id].sLock);
					clients[client_id].state = ST_ALLOC;
				}
				clients[client_id].x = (rand() % 400);
				clients[client_id].y = (rand() % 400);
				clients[client_id].id = client_id;
				clients[client_id].name[0] = 0;
				clients[client_id].prevRemain = 0;
				clients[client_id].socket = g_c_socket;
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_c_socket), hIocp, client_id, 0);
				clients[client_id].DoRecv();
				g_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			}
			else {
				cout << "Max user exceeded.\n";
			}
			ZeroMemory(&g_a_over.over, sizeof(g_a_over.over));
			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(g_s_socket, g_c_socket, g_a_over.sendBuf, 0, addr_size + 16, addr_size + 16, 0, &g_a_over.over);
			break;
		}
		case RECV: {
			int remain_data = numBytes + clients[key].prevRemain;
			char* p = exOver->sendBuf;
			while (remain_data > 0) {
				int packet_size = p[0];
				if (packet_size <= remain_data) {
					ProcessPacket(static_cast<int>(key), p);
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;
			}
			clients[key].prevRemain = remain_data;
			if (remain_data > 0) {
				memcpy(exOver->sendBuf, p, remain_data);
			}
			clients[key].DoRecv();
			break;
		}
		case SEND:
			delete exOver;
			break;
		}

	}
}

int main() {
	HANDLE hIocp;
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	g_s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT_NUM);
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(g_s_socket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
	listen(g_s_socket, SOMAXCONN);

	SOCKADDR_IN cAddr;
	int addrSize = sizeof(cAddr);
	int cId = 0;

	hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_s_socket), hIocp, 9999, 0);
	g_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_a_over.compType = ACCEPT;

	AcceptEx(g_s_socket, g_c_socket, g_a_over.sendBuf, 0, addrSize + 16, addrSize + 16, 0, &g_a_over.over);

	vector<thread> workerThreads;

	int numThreads = thread::hardware_concurrency();
	for (int i = 0; i < numThreads; ++i)
		workerThreads.emplace_back(WorkerThread, hIocp);
	for (auto& th : workerThreads)
		th.join();

	closesocket(g_s_socket);
	WSACleanup();
}