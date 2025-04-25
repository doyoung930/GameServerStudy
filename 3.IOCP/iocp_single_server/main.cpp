#include <iostream>
#include <array>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include "..\protocol.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;

constexpr int MAX_USER = 10000;

enum CompletionType { Accept, Recv, Send};

class OverlappedEx {
public:
	WSAOVERLAPPED overlapped;
	WSABUF wsaBuf;
	char buffer[BUF_SIZE];
	CompletionType compType;
	OverlappedEx()
	{
		wsaBuf.len = BUF_SIZE;
		wsaBuf.buf = buffer;
		compType = Recv;
		ZeroMemory(&overlapped, sizeof(overlapped));
	}
	OverlappedEx(char* packet)
	{
		wsaBuf.len = packet[0];
		wsaBuf.buf = buffer;
		compType = Send;
		ZeroMemory(&overlapped, sizeof(overlapped));
		
		memcpy(buffer, packet, packet[0]);
	}
};

class Session {
	OverlappedEx recvOver;

public:
	bool	inUse;
	int		id;
	SOCKET socket;
	short	posX, posY;
	char	name[NAME_SIZE];
	int		prevRemain;
	int		lastMoveTime;

	Session() {
		id = -1;
		socket = 0;
		posX = posY = 0;
		name[0] = 0;
		inUse = false;
		prevRemain = 0;
	}
	
	~Session() {
	}

	void doReceive() {
		DWORD recvFlag = 0;
		memset(&recvOver.overlapped, 0, sizeof(recvOver.overlapped));
		recvOver.wsaBuf.len = BUF_SIZE - prevRemain;
		recvOver.wsaBuf.buf = recvOver.buffer + prevRemain;
		WSARecv(socket, &recvOver.wsaBuf, 1, 0, &recvFlag, &recvOver.overlapped, 0);
	}

	void doSend(void* packet) {
		OverlappedEx* sendData = new OverlappedEx{ reinterpret_cast<char*>(packet) };
		WSASend(socket, &sendData->wsaBuf, 1, 0, 0, &sendData->overlapped, 0);
	}

	void sendLoginInfoPacket() {
		SC_LOGIN_INFO_PACKET packet;

		packet.id = id;
		packet.size = sizeof(SC_LOGIN_INFO_PACKET);
		packet.type = SC_LOGIN_INFO;
		packet.x = posX;
		packet.y = posY;
		doSend(&packet);
	}

	void sendMovePacket(int clientId);
};

array<Session, MAX_USER> g_clients;

void Session::sendMovePacket(int clientId) {
	SC_MOVE_PLAYER_PACKET packet;

	packet.id = clientId;
	packet.size = sizeof(SC_MOVE_PLAYER_PACKET);
	packet.type = SC_MOVE_PLAYER;
	packet.x = g_clients[clientId].posX;
	packet.y = g_clients[clientId].posY;
	packet.move_time = g_clients[clientId].lastMoveTime;
	doSend(&packet);
}

int getNewClientId() {
	for (int i = 0; i < MAX_USER; ++i)
		if (!g_clients[i].inUse)
			return i;
	return -1;
}

void processPacket(int clientId, char* packet) {
	switch (packet[1]) {
	case CS_LOGIN: {
			CS_LOGIN_PACKET * loginPacket = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
			strcpy_s(g_clients[clientId].name, loginPacket->name);
			g_clients[clientId].sendLoginInfoPacket();

			for (auto& client : g_clients) {
				if (!client.inUse || client.id == clientId) continue;

				SC_ADD_PLAYER_PACKET addPacket;
				addPacket.id = clientId;
				strcpy_s(addPacket.name, loginPacket->name);
				addPacket.size = sizeof(addPacket);
				addPacket.type = SC_ADD_PLAYER;
				addPacket.x = g_clients[clientId].posX;
				addPacket.y = g_clients[clientId].posY;

				client.doSend(&addPacket);
			}

			for (auto& client : g_clients) {
				if (!client.inUse || client.id == clientId) continue;

				SC_ADD_PLAYER_PACKET addPacket;
				addPacket.id = client.id;
				strcpy_s(addPacket.name, client.name);
				addPacket.size = sizeof(addPacket);
				addPacket.type = SC_ADD_PLAYER;
				addPacket.x = client.posX;
				addPacket.y = client.posY;

				g_clients[clientId].doSend(&addPacket);
			}
			break;
		}
	case CS_MOVE: {
		CS_MOVE_PACKET* movePacket = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		g_clients[clientId].lastMoveTime = movePacket->move_time;
		short x = g_clients[clientId].posX;
		short y = g_clients[clientId].posY;

		switch (movePacket->direction) {
		case 0: if (y > 0) y--; break;
		case 1: if (y < W_HEIGHT - 1) y++; break;
		case 2: if (x > 0) x--; break;
		case 3: if (x < W_WIDTH - 1) x++; break;
		}

		g_clients[clientId].posX = x;
		g_clients[clientId].posY = y;

		for (auto& client : g_clients)
			if (client.inUse)
				client.sendMovePacket(clientId);

		break;
	}
	}
}

void disconnectClient(int clientId) {
	for (auto& client : g_clients) {
		if (!client.inUse || client.id == clientId) continue;
		SC_REMOVE_PLAYER_PACKET packet;
		packet.id = clientId;
		packet.size = sizeof(packet);
		packet.type = SC_REMOVE_PLAYER;
		client.doSend(&packet);
	}
	closesocket(g_clients[clientId].socket);
	g_clients[clientId].inUse = false;
}

int main() {
	HANDLE iocpHandle;

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET serverSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN serverAddr = {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT_NUM);
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
	listen(serverSocket, SOMAXCONN);

	SOCKADDR_IN clientAddr;
	int addrSize = sizeof(clientAddr);
	SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	OverlappedEx acceptOver;
	acceptOver.compType = CompletionType::Accept;

	iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(serverSocket), iocpHandle, 9999, 0);

	AcceptEx(serverSocket, clientSocket, acceptOver.buffer, 0, addrSize + 16, addrSize + 16, 0, &acceptOver.overlapped);

	while (true) {
		DWORD numBytes;
		ULONG_PTR key;
		WSAOVERLAPPED* overlapped = nullptr;
		BOOL result = GetQueuedCompletionStatus(iocpHandle, &numBytes, &key, &overlapped, INFINITE);

		OverlappedEx* ex = reinterpret_cast<OverlappedEx*>(overlapped);

		if (!result) {
			if (ex->compType == CompletionType::Accept)
				cout << "Accept Error";
			else {
				cout << "GQCS Error on client[" << key << "]\n";
				disconnectClient(static_cast<int>(key));
				if (ex->compType == CompletionType::Send) delete ex;
				continue;
			}
		}

		switch (ex->compType) {
		case CompletionType::Accept: {
			int newClientId = getNewClientId();
			if (newClientId != -1) {
				g_clients[newClientId].inUse = true;
				g_clients[newClientId].posX = rand() % 400;
				g_clients[newClientId].posY = rand() % 400;
				g_clients[newClientId].id = newClientId;
				g_clients[newClientId].name[0] = 0;
				g_clients[newClientId].prevRemain = 0;
				g_clients[newClientId].socket = clientSocket;

				CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), iocpHandle, newClientId, 0);
				g_clients[newClientId].doReceive();

				clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			}
			else {
				cout << "Max user exceeded.\n";
			}

			ZeroMemory(&acceptOver.overlapped, sizeof(acceptOver.overlapped));
			AcceptEx(serverSocket, clientSocket, acceptOver.buffer, 0, addrSize + 16, addrSize + 16, 0, &acceptOver.overlapped);
			break;
		}
		case CompletionType::Recv: {
			int remainData = numBytes + g_clients[key].prevRemain;
			char* ptr = ex->buffer;

			while (remainData > 0) {
				int packetSize = ptr[0];
				if (packetSize <= remainData) {
					processPacket(static_cast<int>(key), ptr);
					ptr += packetSize;
					remainData -= packetSize;
				}
				else {
					break;
				}
			}

			g_clients[key].prevRemain = remainData;
			if (remainData > 0)
				memcpy(ex->buffer, ptr, remainData);

			g_clients[key].doReceive();
			break;
		}
		case CompletionType::Send:
			delete ex;
			break;
		}
	}

	closesocket(serverSocket);
	WSACleanup();
}
