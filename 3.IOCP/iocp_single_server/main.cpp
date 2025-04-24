#include <iostream>
#include <array>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include "..\protocol.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;

constexpr int MAX_USER = 10000;

enum COMP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND };

class OVER_EXP {
public:
	WSAOVERLAPPED _over;
	WSABUF _wsabuf;
	char _buf[BUF_SIZE];
	COMP_TYPE _comp_type;
	OVER_EXP()
	{
		_wsabuf.len = BUF_SIZE;
		_wsabuf.buf = _buf;
		_comp_type = OP_RECV;
		ZeroMemory(&_over, sizeof(_over));
	}
	OVER_EXP(char* packet)
	{
		_wsabuf.len = packet[0];
		_wsabuf.buf = _buf;
		_comp_type = OP_SEND;
		ZeroMemory(&_over, sizeof(_over));
		
		memcpy(_buf, packet, packet[0]);
	}
};