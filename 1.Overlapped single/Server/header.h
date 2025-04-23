#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <iostream>
#include <WS2tcpip.h>
//#include <ws2tcpip.h>
using namespace std;
#pragma comment (lib, "WS2_32.LIB")

const short SERVER_PORT = 9000;
const int BUFSIZE = 256;


// 에러검사
void err_display(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    printf("[%s] ", msg);
    std::wcout << L"에러 " << (LPTSTR)lpMsgBuf << std::endl;
    LocalFree(lpMsgBuf);
}

typedef struct move_pakcet {
    short size;
    char type;
    int x;
    int y;
}MOVEPACKET;

enum class MOVETYPE
{
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT
};
