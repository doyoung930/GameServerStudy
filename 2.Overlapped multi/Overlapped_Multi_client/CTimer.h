#pragma once
#include"header.h"
class CTimer
{
	CTimer();
	~CTimer();
private:
	static CTimer* m_pInst;

public:
	static CTimer* GetInst()
	{
		if (!m_pInst)
			m_pInst = new CTimer;
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
	LARGE_INTEGER m_tSecond;
	LARGE_INTEGER m_tTime;
	float m_fDeltaTime;
	int m_iFrameMax;
	int m_iFrame;

public:
	float GetDeltaTime() const
	{
		return m_fDeltaTime;
	}
public:
	bool Init();
	void Update();
};

