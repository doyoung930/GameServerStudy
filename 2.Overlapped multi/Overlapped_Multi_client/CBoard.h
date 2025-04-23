#pragma once
#include"header.h"
class CBoard
{
private:
	CBoard();
	~CBoard();
private:
	static CBoard* m_pInst;
	
public:
	static CBoard* GetInst()
	{
		if (!m_pInst)
			m_pInst = new CBoard;
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
	RECT m_Tile[8][8];
	int x, y;
public:
	int GetX()
	{
		return x;
	}
	int GetY()
	{
		return y;
	}

	void SetX(int val)
	{
		x = val;
	}
	void SetY(int val)
	{
		y = val;
	}
	RECT GetSize(int x, int y)
	{
		return m_Tile[x][y];
	}
	
	void Render(HDC hdc, float fDeltaTime);

};

