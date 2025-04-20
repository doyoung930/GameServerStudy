#pragma once
#include <WS2tcpip.h>
#include <Windows.h>
#include <iostream>
#include "resource.h"
#include "type.h"
#pragma comment (lib,"WS2_32.LIB")
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

using namespace std;

//const char* SERVER_ADDR = "127.0.0.1";
const short SERVER_PORT = 4000;
