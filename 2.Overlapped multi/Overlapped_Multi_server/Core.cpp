#include "Core.h"
#include"Server.h"
Core* Core::m_pInst = NULL;
Core::Core()
{
}

Core::~Core()
{
}

bool Core::Init()
{
    if (!Server::GetInstance()->Initialize())
        return false;
    return true;
}

int Core::Run()
{
    Server::GetInstance()->Run();

    return 0;
}
