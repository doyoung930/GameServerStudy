#pragma once
#include<WS2tcpip.h>//���� �������� �����ε��Ǿ� �浹���� ������ ���������� ����
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

