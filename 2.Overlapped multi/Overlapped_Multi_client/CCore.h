#pragma once

#include"header.h"

class CCore
{
private:
	static CCore* m_pInst;
	static bool m_bLoop;
public:
	static CCore* GetInst()
	{
		if (!m_pInst)
			m_pInst = new CCore;
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
	CCore();
	~CCore();

private:
	HINSTANCE m_hInst;
	HWND m_hWnd;
	HDC m_hDC;
	RESOLUTION m_tRS;
	bool bOnce = false;
public:
	bool Init(HINSTANCE hInst);
	int Run();
private:
	ATOM MyRegisterClass();
	BOOL Create();
	void Input(float fDeltaTime);
	void Update(float fDeltaTime);
	void Render(float fDeltaTime);
public:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

