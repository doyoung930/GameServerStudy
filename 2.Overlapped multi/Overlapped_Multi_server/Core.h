#pragma once
#include"header.h"
class Core
{
private:
	static Core* m_pInst;
	static bool m_bLoop;
public:
	static Core* GetInst()
	{
		if (!m_pInst)
			m_pInst = new Core;
		return m_pInst;
	}

	static void DestroyInst()
	{
		if (m_pInst)
		{
			delete m_pInst;
			m_pInst = NULL;
		}
	}

private:
	Core();
	~Core();
public:
	bool Init();
	int Run();
};

