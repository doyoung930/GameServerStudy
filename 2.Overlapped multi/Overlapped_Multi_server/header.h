#pragma once

#include <iostream>
#include <WS2tcpip.h>
#include "type.h"
#include <unordered_map>

using namespace std;

#pragma comment (lib,"WS2_32.LIB")

static const short SERVER_PORT = 9000;

static const int BUFSIZE = 512;
