#pragma once
#include<WS2tcpip.h>//옛날 버전에서 오버로딩되어 충돌나기 때문에 무조건위에 선언
#include<Windows.h>
#include<iostream>
#include<unordered_map>
#include"resource.h"
#include"type.h"
#pragma comment (lib,"WS2_32.LIB")
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

using namespace std;


static const int BUFSIZE = 512;
static const short SERVER_PORT = 9000;

